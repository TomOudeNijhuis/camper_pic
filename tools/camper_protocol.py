#!/usr/bin/env python3
"""Host-side helper for the Camper PIC binary protocol.

Frame: [SOF=0xAA][OPCODE][LEN][PAYLOAD...][CRC_LO][CRC_HI]
CRC over OPCODE+LEN+PAYLOAD using CRC-16/MCRF4XX (poly 0x1021,
init 0xFFFF, refin=true, refout=true, xorout=0x0000).

Usage (default port /dev/ttyS0; override with --port /dev/ttyUSB0):
    python camper_protocol.py ping
    python camper_protocol.py get-household
    python camper_protocol.py set-pump 1
    python camper_protocol.py subscribe
    python camper_protocol.py stream                  # listen & decode
    python camper_protocol.py --port /dev/ttyUSB0 ping
"""

from __future__ import annotations

import argparse
import struct
import sys
import time
from dataclasses import dataclass

import serial  # pip install pyserial


SOF = 0xAA
MAX_PAYLOAD = 56

# Opcodes -- keep in sync with protocol.h
OP_GET_HOUSEHOLD    = 0x10
OP_GET_PUMP         = 0x11
OP_GET_VOLTAGE      = 0x12
OP_GET_WATER        = 0x13
OP_GET_WASTE        = 0x14
OP_GET_ALL          = 0x15
OP_SET_HOUSEHOLD    = 0x20
OP_SET_PUMP         = 0x21
OP_SET_NEOPIXEL     = 0x22
OP_SUBSCRIBE        = 0x30
# 0x31 reserved (was OP_HEARTBEAT; OP_SUBSCRIBE is now idempotent)
OP_UNSUBSCRIBE      = 0x32
OP_ENTER_BOOTLOADER = 0x40
OP_PING             = 0x41
OP_VERSION          = 0x42
OP_GET_ERRORS       = 0x43
OP_CLEAR_ERRORS     = 0x44
OP_TELEMETRY_PUSH   = 0x80
OP_EVENT            = 0x81
OP_ACK              = 0xF0
OP_NACK             = 0xF1

NACK_REASONS = {
    0x01: "BAD_CRC",
    0x02: "BAD_LEN",
    0x03: "UNKNOWN_OPCODE",
    0x04: "BAD_PARAM",
    0x05: "BUSY",
}

ERROR_BITS = {
    0x0001: "HOUSEHOLD_SWITCH_FAILED",
    0x0002: "VOLTAGE_HOUSEHOLD_LOW",
    0x0004: "VOLTAGE_STARTER_LOW",
    0x0008: "VOLTAGE_MAINS_LOW",
    0x0010: "WATER_LOW",
    0x0020: "WASTE_HIGH",
    0x0040: "WASTE_FULL",
    0x0080: "ADC_STUCK",
    0x0100: "PROTOCOL_CRC",
    0x0200: "PROTOCOL_OVERRUN",
    0x0400: "BROWN_OUT",
}


def crc16_mcrf4xx(data: bytes) -> int:
    """CRC-16/MCRF4XX. Reflected poly 0x8408, init 0xFFFF."""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0x8408
            else:
                crc >>= 1
    return crc & 0xFFFF


def build_frame(opcode: int, payload: bytes = b"") -> bytes:
    if len(payload) > MAX_PAYLOAD:
        raise ValueError(f"payload too long: {len(payload)}")
    body = bytes([opcode, len(payload)]) + payload
    crc = crc16_mcrf4xx(body)
    return bytes([SOF]) + body + bytes([crc & 0xFF, (crc >> 8) & 0xFF])


@dataclass
class Frame:
    opcode: int
    payload: bytes

    def __repr__(self) -> str:
        return f"Frame(op=0x{self.opcode:02X}, payload={self.payload.hex()})"


class FrameParser:
    """Stateful byte-by-byte parser, mirrors the firmware state machine."""

    IDLE, GOT_SOF, GOT_OP, IN_PAYLOAD, GOT_CRC_LO = range(5)

    def __init__(self) -> None:
        self.state = self.IDLE
        self.opcode = 0
        self.length = 0
        self.payload = bytearray()
        self.crc_lo = 0

    def feed(self, b: int) -> Frame | None:
        if self.state == self.IDLE:
            if b == SOF:
                self.state = self.GOT_SOF
                self.payload.clear()
            return None
        if self.state == self.GOT_SOF:
            self.opcode = b
            self.state = self.GOT_OP
            return None
        if self.state == self.GOT_OP:
            if b > MAX_PAYLOAD:
                self.state = self.IDLE
                return None
            self.length = b
            self.state = self.IN_PAYLOAD if b else self.GOT_CRC_LO
            return None
        if self.state == self.IN_PAYLOAD:
            self.payload.append(b)
            if len(self.payload) >= self.length:
                self.state = self.GOT_CRC_LO
            return None
        if self.state == self.GOT_CRC_LO:
            self.crc_lo = b
            self.state = self.IDLE  # next byte completes frame
            self._next_is_crc_hi = True
            return None
        return None

    def feed_stream(self, data: bytes):
        out = []
        i = 0
        # Re-implement the state machine inline so we can take two CRC bytes
        # without juggling extra parser state.
        while i < len(data):
            b = data[i]
            i += 1
            if self.state == self.IDLE:
                if b == SOF:
                    self.state = self.GOT_SOF
                    self.payload.clear()
                continue
            if self.state == self.GOT_SOF:
                self.opcode = b
                self.state = self.GOT_OP
                continue
            if self.state == self.GOT_OP:
                if b > MAX_PAYLOAD:
                    self.state = self.IDLE
                    continue
                self.length = b
                self.state = self.IN_PAYLOAD if b else self.GOT_CRC_LO
                continue
            if self.state == self.IN_PAYLOAD:
                self.payload.append(b)
                if len(self.payload) >= self.length:
                    self.state = self.GOT_CRC_LO
                continue
            if self.state == self.GOT_CRC_LO:
                self.crc_lo = b
                self.state = 99  # waiting for CRC hi
                continue
            if self.state == 99:
                received = self.crc_lo | (b << 8)
                body = bytes([self.opcode, self.length]) + bytes(self.payload)
                if received == crc16_mcrf4xx(body):
                    out.append(Frame(self.opcode, bytes(self.payload)))
                # else: silently drop, firmware will NACK
                self.state = self.IDLE
        return out


class CamperLink:
    def __init__(self, port: str, baud: int = 115200, timeout: float = 1.0) -> None:
        self.ser = serial.Serial(port, baud, timeout=timeout)
        self.parser = FrameParser()

    def send(self, opcode: int, payload: bytes = b"") -> None:
        frame = build_frame(opcode, payload)
        self.ser.write(frame)

    def recv(self, expect: int | None = None, timeout: float = 1.0) -> Frame:
        end = time.monotonic() + timeout
        while time.monotonic() < end:
            n = self.ser.in_waiting or 1
            data = self.ser.read(n)
            if not data:
                continue
            for f in self.parser.feed_stream(data):
                if expect is None or f.opcode == expect:
                    return f
                print(f"[skipped] {decode_frame(f)}", file=sys.stderr)
        raise TimeoutError(f"no frame (expected opcode=0x{(expect or 0):02X})")

    def request(self, opcode: int, payload: bytes = b"", timeout: float = 1.0) -> Frame:
        self.send(opcode, payload)
        return self.recv(timeout=timeout)


def decode_frame(f: Frame) -> str:
    if f.opcode == OP_ACK:
        op_acked = f.payload[0] if f.payload else None
        body = f.payload[1:].hex()
        return f"ACK op=0x{op_acked:02X} data={body}"
    if f.opcode == OP_NACK:
        op_acked = f.payload[0] if f.payload else None
        reason = f.payload[1] if len(f.payload) > 1 else None
        return f"NACK op=0x{op_acked:02X} reason={NACK_REASONS.get(reason, hex(reason))}"
    if f.opcode == OP_TELEMETRY_PUSH:
        return decode_telemetry(f.payload)
    if f.opcode == OP_EVENT:
        if f.payload and f.payload[0] == 0x01:
            mask = struct.unpack("<H", f.payload[1:3])[0]
            return f"EVENT ERROR_RAISED mask=0x{mask:04X} bits={decode_errors(mask)}"
        if f.payload and f.payload[0] == 0x02 and len(f.payload) >= 7:
            rx_bytes, frames, crc_fails = struct.unpack("<HHH", f.payload[1:7])
            return f"EVENT DEBUG rx_bytes={rx_bytes} frames_ok={frames} crc_fails={crc_fails}"
        return f"EVENT kind={f.payload[0]:02X} body={f.payload[1:].hex()}"
    return f"{f}"


def decode_telemetry(p: bytes) -> str:
    if len(p) != 11:
        return f"TELEMETRY (bad len {len(p)}): {p.hex()}"
    vh, vm, vs, water, waste, flags, errs = struct.unpack("<HHHBBBH", p)
    return (f"TELEMETRY vh={vh}mV vm={vm}mV vs={vs}mV "
            f"water={water}% waste={waste}% "
            f"flags=0x{flags:02X} errors=0x{errs:04X} [{decode_errors(errs)}]")


def decode_errors(mask: int) -> str:
    return ",".join(name for bit, name in ERROR_BITS.items() if mask & bit) or "-"


# ---------- subcommands ----------

def cmd_ping(link: CamperLink, args) -> None:
    payload = args.payload.encode() if args.payload else b"PING"
    f = link.request(OP_PING, payload)
    print(decode_frame(f))


def cmd_get_household(link: CamperLink, args) -> None:
    f = link.request(OP_GET_HOUSEHOLD)
    print(decode_frame(f))


def cmd_set_household(link: CamperLink, args) -> None:
    f = link.request(OP_SET_HOUSEHOLD, bytes([int(args.state)]))
    print(decode_frame(f))


def cmd_set_pump(link: CamperLink, args) -> None:
    f = link.request(OP_SET_PUMP, bytes([int(args.state)]))
    print(decode_frame(f))


def cmd_get_voltage(link: CamperLink, args) -> None:
    ch_map = {"household": 0, "mains": 1, "starter": 2}
    ch = ch_map.get(args.channel, int(args.channel))
    f = link.request(OP_GET_VOLTAGE, bytes([ch]))
    if f.opcode == OP_ACK and len(f.payload) >= 4:
        ch_back = f.payload[1]
        mv = struct.unpack("<H", f.payload[2:4])[0]
        print(f"ACK channel={ch_back} voltage={mv}mV")
    else:
        print(decode_frame(f))


def cmd_get_all(link: CamperLink, args) -> None:
    f = link.request(OP_GET_ALL)
    if f.opcode == OP_ACK:
        print(decode_telemetry(f.payload[1:]))
    else:
        print(decode_frame(f))


def cmd_subscribe(link: CamperLink, args) -> None:
    f = link.request(OP_SUBSCRIBE, bytes([0]))
    print(decode_frame(f))
    if args.stream:
        cmd_stream(link, args)


def cmd_unsubscribe(link: CamperLink, args) -> None:
    f = link.request(OP_UNSUBSCRIBE)
    print(decode_frame(f))


def cmd_version(link: CamperLink, args) -> None:
    f = link.request(OP_VERSION)
    if f.opcode == OP_ACK and len(f.payload) >= 4:
        print(f"VERSION {f.payload[1]}.{f.payload[2]}.{f.payload[3]}")
    else:
        print(decode_frame(f))


def cmd_get_errors(link: CamperLink, args) -> None:
    f = link.request(OP_GET_ERRORS)
    if f.opcode == OP_ACK and len(f.payload) >= 3:
        mask = struct.unpack("<H", f.payload[1:3])[0]
        print(f"ERRORS mask=0x{mask:04X} [{decode_errors(mask)}]")
    else:
        print(decode_frame(f))


def cmd_clear_errors(link: CamperLink, args) -> None:
    mask = int(args.mask, 0)
    f = link.request(OP_CLEAR_ERRORS, struct.pack("<H", mask))
    if f.opcode == OP_ACK and len(f.payload) >= 3:
        remaining = struct.unpack("<H", f.payload[1:3])[0]
        print(f"CLEARED. remaining=0x{remaining:04X} [{decode_errors(remaining)}]")
    else:
        print(decode_frame(f))


def cmd_set_neopixel(link: CamperLink, args) -> None:
    color_map = {
        "red": 0, "green": 1, "blue": 2, "yellow": 3, "magenta": 4,
        "cyan": 5, "white": 6, "maroon": 7, "dark_green": 8, "navy": 9,
        "olive": 10, "purple": 11, "teal": 12, "silver": 13, "gray": 14,
        "black": 15,
    }
    color = color_map[args.color] if args.color in color_map else int(args.color, 0)
    payload = bytes([int(args.led), color, int(args.duty)])
    f = link.request(OP_SET_NEOPIXEL, payload)
    print(decode_frame(f))


def cmd_enter_bootloader(link: CamperLink, args) -> None:
    if not args.yes:
        print("Refusing without --yes. This will reset the board into the bootloader.")
        return
    f = link.request(OP_ENTER_BOOTLOADER, bytes([0x5A]))
    print(decode_frame(f))


def cmd_stream(link: CamperLink, args) -> None:
    """Listen and decode async frames.

    If --keepalive is set (default 3s), re-sends OP_SUBSCRIBE on that
    interval to keep the firmware in fast-streaming mode. SUBSCRIBE is
    idempotent: only the first call starts streaming; subsequent calls
    just refresh the firmware's 10s watchdog.
    """
    print("Streaming. Press Ctrl-C to stop.")
    last_keepalive = 0.0
    try:
        while True:
            now = time.monotonic()
            if args.keepalive and now - last_keepalive >= args.keepalive:
                link.send(OP_SUBSCRIBE, bytes([0]))
                last_keepalive = now
            data = link.ser.read(64)
            if data:
                for f in link.parser.feed_stream(data):
                    print(f"[{time.strftime('%H:%M:%S')}] {decode_frame(f)}")
    except KeyboardInterrupt:
        print()


SUBCOMMANDS = {
    "ping":             (cmd_ping,             [("--payload", {"default": None})]),
    "get-household":    (cmd_get_household,    []),
    "set-household":    (cmd_set_household,    [("state", {})]),
    "get-pump":         (lambda l,a: print(decode_frame(l.request(OP_GET_PUMP))), []),
    "set-pump":         (cmd_set_pump,         [("state", {})]),
    "get-voltage":      (cmd_get_voltage,      [("channel", {})]),
    "get-water":        (lambda l,a: print(decode_frame(l.request(OP_GET_WATER))), []),
    "get-waste":        (lambda l,a: print(decode_frame(l.request(OP_GET_WASTE))), []),
    "get-all":          (cmd_get_all,          []),
    "set-neopixel":     (cmd_set_neopixel,     [("led", {}), ("color", {}), ("duty", {})]),
    "subscribe":        (cmd_subscribe,        [("--stream", {"action": "store_true"}),
                                                ("--keepalive", {"type": float, "default": 3.0})]),
    "unsubscribe":      (cmd_unsubscribe,      []),
    "version":          (cmd_version,          []),
    "get-errors":       (cmd_get_errors,       []),
    "clear-errors":     (cmd_clear_errors,     [("mask", {"default": "0xFFFF", "nargs": "?"})]),
    "enter-bootloader": (cmd_enter_bootloader, [("--yes", {"action": "store_true"})]),
    "stream":           (cmd_stream,           [("--keepalive", {"type": float, "default": 3.0})]),
}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", "-p", default="/dev/ttyS0", help="serial port (default: /dev/ttyS0)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name, (fn, posargs) in SUBCOMMANDS.items():
        sp = sub.add_parser(name)
        for argname, kw in posargs:
            sp.add_argument(argname, **kw)
        sp.set_defaults(func=fn)

    args = ap.parse_args()
    link = CamperLink(args.port)
    args.func(link, args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
