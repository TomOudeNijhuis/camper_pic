# Camper PIC binary protocol

Wire spec for the host. Firmware reference: [protocol.h](My_MCC_Config/mcc/drivers/protocol.h),
working host implementation in [tools/camper_protocol.py](tools/camper_protocol.py).

## Transport

- UART, **115200 baud, 8N1**, no flow control.
- Half-duplex from the host point of view: send a command frame, read the matching reply (`OP_ACK` or `OP_NACK`).
- The firmware may also push **async frames** (`OP_TELEMETRY_PUSH`, `OP_EVENT`) at any time. The host must be ready to interleave async frames with command replies — keep a stream parser running continuously, not only after each request.

## Frame format

```
Offset  Size  Field      Notes
0       1     SOF        0xAA (sync byte; not included in CRC)
1       1     OPCODE     see Opcodes
2       1     LEN        payload length, 0..56
3..2+L  L     PAYLOAD    opcode-specific
3+L     1     CRC_LO     CRC-16/MCRF4XX, little-endian
4+L     1     CRC_HI
```

Max frame size = 4 + 56 + 2 = **62 bytes**.

- CRC is computed over **`[OPCODE, LEN, PAYLOAD...]`** (4 bytes of header+CRC excluded? — to be precise: it covers the OPCODE byte, the LEN byte, and the LEN payload bytes; the SOF and the CRC bytes themselves are not included).
- CRC bytes are transmitted **little-endian** (low byte first).
- All multi-byte integers inside payloads are **little-endian** unless stated otherwise.
- If `LEN > 56`, the firmware resets the parser to IDLE and drops the frame — there is no error reply (the byte stream might be garbage). The host should never produce such frames; any host-side fuzz test should treat them as forbidden.

### CRC-16/MCRF4XX

Standard parameters:

| Param | Value |
|---|---|
| Width | 16 |
| Poly | 0x1021 |
| Init | 0xFFFF |
| RefIn | true |
| RefOut | true |
| XorOut | 0x0000 |

Reflected polynomial = `0x8408`. The bootloader uses the same CRC for image verification, so a single routine serves both.

Reference Python implementation:

```python
def crc16_mcrf4xx(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0x8408 if crc & 1 else crc >> 1
    return crc & 0xFFFF
```

Test vectors (verify your implementation against these):

| Input bytes (hex) | CRC-16 |
|---|---|
| `41 04 DE AD BE EF` | `0x3A8F` |
| `41 04 50 49 4E 47` | `0x8629` |
| `21 01 01` | `0x6885` |
| `30 01 00` | `0xA645` |
| `40 01 5A` | `0xDB42` |

## Stream parser

The firmware's parser is a forgiving 7-state machine; a compatible host parser must:

1. Drop bytes while in IDLE until `0xAA` is seen.
2. Read OPCODE.
3. Read LEN; if `LEN > 56`, return to IDLE (this can only happen on bus garbage or a buggy peer).
4. Read LEN payload bytes.
5. Read CRC_LO, then CRC_HI.
6. Verify CRC over `OPCODE || LEN || PAYLOAD`. If mismatch, **drop the frame silently** and return to IDLE.
7. On success, emit the frame to the application layer.

Resync is automatic: after any bad frame the parser is back in IDLE looking for the next `0xAA`.

A stray `0xAA` inside a payload won't false-trigger the parser as long as the surrounding bytes form a valid frame and the CRC checks. If the host's first read happens mid-frame, the parser will simply discard bytes until the next valid frame boundary.

## Opcodes

Opcodes are grouped by range so new codes can be added without colliding with NACK reasons or replies.

| Range | Purpose |
|---|---|
| 0x00      | Reserved (NOP) |
| 0x10–0x1F | Getters (request → ACK with data) |
| 0x20–0x2F | Setters (request → ACK confirming new state) |
| 0x30–0x3F | Subscription / lifecycle |
| 0x40–0x4F | System |
| 0x80–0x8F | **Async pushes** (firmware-initiated) |
| 0xF0–0xFF | Replies |

### 0x10 OP_GET_HOUSEHOLD

Request: payload empty.
Reply: `OP_ACK`, payload `[0x10, state]` where `state ∈ {0=OFF, 1=ON}`.

`ON` is reported the moment `SET_HOUSEHOLD 1` is processed (the firmware mirrors its internal marker, no voltage gating in the getter). If the household voltage fails to rise above ~9000 mV within 30 s, the firmware automatically pulses OFF and raises `ERR_HOUSEHOLD_SWITCH_FAILED` — at which point this command returns `0=OFF`.

### 0x11 OP_GET_PUMP

Request: payload empty.
Reply: `OP_ACK`, payload `[0x11, state]` where `state ∈ {0, 1}`.

### 0x12 OP_GET_VOLTAGE

Request: payload `[channel]` where `channel ∈ {0=household, 1=mains, 2=starter}`.
Reply: `OP_ACK`, payload `[0x12, channel, mV_lo, mV_hi]`.

The value is the cached voltage from the last ADC sweep (sweeps run every ~500 ms). It does not block while sampling — you may get the same value for two consecutive requests if no fresh sample has completed.

NACKs: `BAD_LEN` if not 1 byte; `BAD_PARAM` if channel > 2.

### 0x13 OP_GET_WATER, 0x14 OP_GET_WASTE

Request: payload empty.
Reply: `OP_ACK`, payload `[opcode, percent]` where `percent ∈ {0, 25, 50, 75, 100}`.

These reads pulse the PULLUP pin for 10 ms each — at 1 Hz polling this adds 20 ms of main-loop blocking per second. Acceptable but not free.

### 0x15 OP_GET_ALL

Request: payload empty.
Reply: `OP_ACK`, payload `[0x15] + telemetry_payload` (1 + 11 = 12 bytes total). Same telemetry layout as `OP_TELEMETRY_PUSH` below — useful for an on-demand snapshot without subscribing.

### 0x20 OP_SET_HOUSEHOLD

Request: payload `[state]` where `state ∈ {0, 1}`.
Reply: `OP_ACK`, payload `[0x20, household_state]` — the resulting state after the command (`ON` after `SET 1`, `OFF` after `SET 0`).

Side effect when `state == 1`: the firmware arms a 30 s voltage watchdog. The household relay is considered confirmed when `V_household` rises above ~9000 mV. If it doesn't within 30 s, the firmware pulses OFF (clearing the marker, so subsequent `GET_HOUSEHOLD` returns `0=OFF`) and latches `ERR_HOUSEHOLD_SWITCH_FAILED` with an async `OP_EVENT`.

NACKs: `BAD_LEN`, `BAD_PARAM` if state > 1.

### 0x21 OP_SET_PUMP

Request: payload `[state]` where `state ∈ {0, 1}`.
Reply: `OP_ACK`, payload `[0x21, pump_state]`.

### 0x22 OP_SET_NEOPIXEL

Request: payload `[led, color, duty]`:
- `led`: 1 or 2 (two onboard LEDs)
- `color`: 0..15 (see [Color enum](#neopixel-colors))
- `duty`: 0..10 (PWM-ish duty cycle, 0 = always off, 10 = always on)

Reply: `OP_ACK`, payload `[0x22]` (no extra data — the firmware just confirms the command was applied).

NACKs: `BAD_LEN`, `BAD_PARAM` if `led ∉ {1,2}` or `color >= 16` or `duty > 10`.

#### Neopixel colors

| Value | Name | RGB |
|---|---|---|
| 0 | RED | (255, 0, 0) |
| 1 | GREEN | (0, 255, 0) |
| 2 | BLUE | (0, 0, 255) |
| 3 | YELLOW | (255, 255, 0) |
| 4 | MAGENTA | (255, 0, 255) |
| 5 | CYAN | (0, 255, 255) |
| 6 | WHITE | (255, 255, 255) |
| 7 | MAROON | (128, 0, 0) |
| 8 | DARK_GREEN | (0, 128, 0) |
| 9 | NAVY | (0, 0, 128) |
| 10 | OLIVE | (128, 128, 0) |
| 11 | PURPLE | (128, 0, 128) |
| 12 | TEAL | (0, 128, 128) |
| 13 | SILVER | (192, 192, 192) |
| 14 | GRAY | (128, 128, 128) |
| 15 | BLACK | (0, 0, 0) |

Note: when the firmware drives the LEDs based on internal state (relay/pump status), it overrides any host-set color after ~3 minutes of inactivity. Set a color again to refresh the override.

### 0x30 OP_SUBSCRIBE

Request: payload `[flags]` where `flags = 0` (reserved for future bits).
Reply: `OP_ACK`, payload `[0x30]`.

**Idempotent.** This single command serves both as subscribe and as keepalive — call it once to start streaming, then repeatedly (e.g. every 3 s) to keep the firmware's 10 s watchdog alive.

Effects:
1. If not already subscribed: firmware enters **fast streaming mode** (`OP_TELEMETRY_PUSH` every ~1 s) and emits the first telemetry frame immediately.
2. If already subscribed: nothing observable happens to the streaming cadence — the existing fast-rate clock continues unchanged.
3. Always: the 10 s watchdog is refreshed.

If the host stops calling `OP_SUBSCRIBE` for more than 10 s, the firmware falls back to **idle mode** (~60 s push interval). Calling `OP_SUBSCRIBE` again after that point starts a fresh fast-streaming session.

Recommended host pattern:
```
while ui_open:
    link.send(OP_SUBSCRIBE)
    sleep(3)
```

### 0x31 reserved

Was `OP_HEARTBEAT`. Removed when `OP_SUBSCRIBE` became idempotent. Sending this opcode now returns `OP_NACK` with reason `UNKNOWN_OPCODE`.

### 0x32 OP_UNSUBSCRIBE

Request: payload empty.
Reply: `OP_ACK`, payload `[0x32]`.

Drops back to idle mode immediately. Good UI manners on app shutdown, though it's not required — the watchdog handles abandoned subscribers within 10 s anyway.

### 0x40 OP_ENTER_BOOTLOADER

Request: payload `[0x5A]` (magic byte to prevent accidental triggers).
Reply: `OP_ACK`, payload `[0x40]`.

**Immediately after sending the ACK** (after a ~20 ms drain delay), the firmware writes `0xA5` to EEPROM `0xF000` and executes a reset. The next boot enters the [camper_pic_bootloader](https://github.com/) which can then accept a new firmware image over `pymdfu`.

NACKs: `BAD_LEN` if payload is not 1 byte; `BAD_PARAM` if the magic byte is not `0x5A`.

To upload a new image after this command: wait ~50 ms for the reset, then start `pymdfu`.

### 0x41 OP_PING

Request: payload = any bytes (0..55 — leave a byte for the opcode echo in the ACK).
Reply: `OP_ACK`, payload `[0x41] + original_payload`.

Useful for link testing and host-side latency measurement.

### 0x42 OP_VERSION

Request: payload empty.
Reply: `OP_ACK`, payload `[0x42, major, minor, patch]`.

Current: `2.0.0`. The version is bumped manually in [commands.c](My_MCC_Config/mcc/drivers/src/commands.c).

### 0x43 OP_GET_ERRORS

Request: payload empty.
Reply: `OP_ACK`, payload `[0x43, errors_lo, errors_hi]` — a 16-bit error word, little-endian.

See [Error catalogue](#error-catalogue) below.

### 0x44 OP_CLEAR_ERRORS

Request: payload `[mask_lo, mask_hi]` — bits set in the mask are cleared in the firmware's error word.
Reply: `OP_ACK`, payload `[0x44, remaining_lo, remaining_hi]` — error word **after** clearing.

To clear all errors, send mask `0xFFFF`. Clearing a bit also resets the "rising edge" snapshot for that bit, so if the underlying condition becomes true again, the firmware will re-raise it and push a fresh `OP_EVENT`.

**Condition-gated bits.** For errors whose underlying physical condition is currently active, the clear is silently **refused**: `WATER_LOW`, `WASTE_HIGH`, `WASTE_FULL`, `VOLTAGE_HOUSEHOLD_LOW`, `VOLTAGE_STARTER_LOW`, `VOLTAGE_MAINS_LOW`, `ADC_STUCK`. The ACK payload carries the resulting error word so the host can see which bits remained set. Historical/one-shot bits (`HOUSEHOLD_SWITCH_FAILED`, `PROTOCOL_CRC`, `PROTOCOL_OVERRUN`, `BROWN_OUT`) are always clearable. A mask that includes both kinds of bits clears the historical ones and leaves the live ones — no NACK is emitted.

### 0x80 OP_TELEMETRY_PUSH (async)

Pushed by the firmware on its own schedule (not in reply to a command). Payload is **11 bytes**:

```
Offset  Size  Field
0       2     v_household_mV     (u16 LE)
2       2     v_mains_mV         (u16 LE)
4       2     v_starter_mV       (u16 LE)
6       1     water_pct          (0, 25, 50, 75, 100)
7       1     waste_pct          (0, 25, 50, 75, 100)
8       1     state_flags        (see below)
9       2     error_flags        (u16 LE; see Error catalogue)
```

`state_flags` byte:

| Bit | Name | Meaning |
|---|---|---|
| 0 | HOUSEHOLD_ON | Household relay is engaged (voltage confirms) |
| 1 | reserved | was HOUSEHOLD_PENDING; the PENDING state was removed |
| 2 | PUMP_ON | Pump relay engaged |
| 3 | SWITCH1 | Physical switch 1 (pump button) — 1 = released, 0 = pressed |
| 4 | SWITCH2 | Physical switch 2 (household button) — 1 = released, 0 = pressed |
| 5 | VALUES_CHANGED | Any field in this frame differs from the previous push |
| 6 | ERRORS_ACTIVE | `error_flags != 0` (summary bit, redundant with the word) |
| 7 | SUBSCRIBED | Firmware currently considers the host subscribed |

Push cadence:

- Subscribed (host calling `OP_SUBSCRIBE` at least every 10 s): every 1 s.
- Idle (never subscribed, or watchdog expired): every 60 s.

Frames may occasionally be dropped if the EUSART TX FIFO is full (TX is non-blocking for telemetry). The next tick retries — this is fine for periodic data.

### 0x81 OP_EVENT (async)

Pushed by the firmware on a state transition. Payload is `[kind, ...kind-specific body]`.

| Kind | Name | Body |
|---|---|---|
| 0x01 | ERROR_RAISED | `[mask_lo, mask_hi]` — u16 mask of bits that **newly** became set since the previous push of this event |
| 0x03 | BOOT_RESET_CAUSE | `[pcon0]` — raw PCON0 byte snapshotted at boot before any software touched it |

Only the *newly raised* bits appear in the mask, not the full current error word. To get the full word after handling the event, call `OP_GET_ERRORS`. Each bit is announced exactly once per rising edge; clearing then re-raising the same condition produces a fresh `ERROR_RAISED` event.

`ERROR_RAISED` is sent **blocking** (firmware will not drop it on TX FIFO pressure) and is independent of the subscribe state — even an idle host gets it immediately, as long as the link is up.

`BOOT_RESET_CAUSE` is sent **once per boot**, blocking, immediately after the UART comes up. The byte's bit layout (PIC16F18045 PCON0):

| Bit | Name | Polarity | Meaning when "this reset happened" |
|---|---|---|---|
| 7 | STKOVF | active-high | `1` = hardware stack overflow |
| 6 | STKUNF | active-high | `1` = hardware stack underflow |
| 4 | nRWDT  | active-low  | `0` = watchdog reset |
| 3 | nRMCLR | active-low  | `0` = MCLR pin reset |
| 2 | nRI    | active-low  | `0` = software `RESET` instruction |
| 1 | nPOR   | active-low  | `0` = power-on reset |
| 0 | nBOR   | active-low  | `0` = brown-out reset |

Firmware clears all sticky flags after snapshotting, so each boot's value reflects only the most recent reset cause.

### 0xF0 OP_ACK

Sent by the firmware in reply to a successful command. Payload always starts with the **opcode being ACKed** so the host can correlate without keeping its own request-reply state machine.

```
payload[0]   = opcode of the original command
payload[1..] = command-specific response data
```

### 0xF1 OP_NACK

Sent on a recognised command that failed validation.

```
payload[0] = opcode of the original command (or 0xFF if unknown)
payload[1] = reason
```

| Reason | Name | Cause |
|---|---|---|
| 0x01 | BAD_CRC | (informational; firmware also auto-sends this when a CRC-failed frame is received with opcode = 0xFF placeholder) |
| 0x02 | BAD_LEN | Payload length wrong for this opcode |
| 0x03 | UNKNOWN_OPCODE | Opcode not in the dispatch table |
| 0x04 | BAD_PARAM | Payload bytes invalid (e.g. state > 1, channel > 2) |
| 0x05 | BUSY | Reserved for future use; not currently emitted |

If a host frame fails CRC, the firmware emits an unsolicited `OP_NACK` with `payload[0] = 0xFF`, `payload[1] = BAD_CRC`. The host can use this as a hint that a request was lost.

## Error catalogue

The `error_flags` u16 in `OP_TELEMETRY_PUSH` (and the reply to `OP_GET_ERRORS`) is a bitmask. Bits are **sticky**: once set, they stay set until the host clears them with `OP_CLEAR_ERRORS`.

| Bit | Mask | Name | Meaning |
|---|---|---|---|
| 0 | 0x0001 | HOUSEHOLD_SWITCH_FAILED | `OP_SET_HOUSEHOLD 1` was issued but `V_household` did not rise above ~9000 mV within 30 s. The firmware also pulses OFF to disengage the relay before latching this bit, so `OP_GET_HOUSEHOLD` returns `0=OFF` after a failure. |
| 1 | 0x0002 | VOLTAGE_HOUSEHOLD_LOW | Household battery voltage < 11800 mV (`GEL_ERROR_TRESHOLD`) for 5 consecutive samples (~2.5 s). |
| 2 | 0x0004 | VOLTAGE_STARTER_LOW | Starter battery voltage < 11000 mV for 5 consecutive samples. |
| 3 | 0x0008 | VOLTAGE_MAINS_LOW | Mains voltage < 11000 mV while household is in ON state (mains expected). |
| 4 | 0x0010 | WATER_LOW | Water tank reading ≤ 25 % for 5 consecutive samples. |
| 5 | 0x0020 | WASTE_HIGH | Waste tank reading ≥ 75 % for 5 consecutive samples. |
| 6 | 0x0040 | WASTE_FULL | Waste tank reading = 100 % (urgent variant; set immediately, no hysteresis). |
| 7 | 0x0080 | ADC_STUCK | ADC sampler hasn't completed a cycle in > 5 s. Indicates firmware or hardware fault. |
| 8 | 0x0100 | PROTOCOL_CRC | A received frame failed CRC. Will fire on link noise — use as diagnostic, not alarm. |
| 9 | 0x0200 | PROTOCOL_OVERRUN | EUSART RX overrun observed. Host sending too fast without flow control, or ISR latency too high. |
| 10 | 0x0400 | BROWN_OUT | The chip's BOR (brown-out reset) latch was set on boot. Battery or supply rail dipped below the BOR threshold. |
| 11–15 | — | reserved | Will read 0; reserve in your host UI for future use. |

Recommended host treatment:

- Tank/voltage errors → user-visible warnings, prompt to acknowledge (which triggers `OP_CLEAR_ERRORS`).
- Protocol errors (CRC, overrun) → silently log + clear; they tend to fire on noise.
- `BROWN_OUT` → show once on first connect after boot, then clear.
- `ADC_STUCK` → escalate, the firmware has likely lost its periodic schedule.

## Worked examples

These show real byte sequences you can paste into a test harness.

### Ping with payload "PING"

Host → device:
```
AA 41 04 50 49 4E 47 29 86
```
- `AA` SOF
- `41` OP_PING
- `04` len=4
- `50 49 4E 47` "PING"
- `29 86` CRC-16/MCRF4XX of `41 04 50 49 4E 47` = `0x8629` → LE `29 86`

Device → host (ACK with opcode echo + payload echo):
```
AA F0 05 41 50 49 4E 47 ?? ??
```
- `F0` OP_ACK
- `05` len=5
- `41` echoed opcode
- `50 49 4E 47` echoed payload
- `?? ??` CRC of `F0 05 41 50 49 4E 47`

### Set pump ON

Host → device: `AA 21 01 01 85 68` (CRC `0x6885` of `21 01 01`).
Device → host (ACK confirming new state): `AA F0 02 21 01 ?? ??`.

### Subscribe (idempotent — also serves as keepalive)

Host: `AA 30 01 00 45 A6` (CRC `0xA645` of `30 01 00`).
Device: `AA F0 01 30 ?? ??` then immediately starts pushing `AA 80 0B <11 telemetry bytes> ?? ??` every second.

Host keeps the stream alive by re-sending the same `AA 30 01 00 45 A6` frame every ~3 seconds. Device ACKs each; the streaming cadence is unchanged by these refreshes — only the 10 s watchdog is reset.

### Enter bootloader

Host: `AA 40 01 5A 42 DB` (CRC `0xDB42` of `40 01 5A`).
Device: `AA F0 01 40 ?? ??`, then ~20 ms later the device resets. The host's UART will see line noise / framing errors briefly while the bootloader starts up.

## Reference implementation

See [tools/camper_protocol.py](tools/camper_protocol.py) for a complete host-side encoder/decoder in Python. The `crc16_mcrf4xx`, `build_frame`, `FrameParser`, and `decode_telemetry` helpers map 1:1 to what's described above and can be transliterated to any host language.
