#include <xc.h>
#include "../protocol.h"
#include "../crc16.h"
#include "../../mcc_generated_files/uart/eusart1.h"

typedef enum {
    RX_IDLE = 0,
    RX_GOT_SOF,
    RX_GOT_OPCODE,
    RX_PAYLOAD,
    RX_CRC_LO,
    RX_CRC_HI
} rx_state_t;

static volatile rx_state_t rx_state = RX_IDLE;
static volatile uint16_t   rx_crc;
static volatile uint8_t    rx_payload_idx;
static volatile uint8_t    rx_crc_lo;

/* Single RX slot. The ISR assembles incoming bytes directly here, so the
 * critical path has no payload-copy loop (an XC8 codegen bug at -O2 makes
 * for-loops inside this ISR's RX_CRC_HI case corrupt the program counter
 * once the function exceeds ~250 bytes). Main reads via protocol_poll(),
 * which also copies it out under GIE=0. */
static volatile protocol_frame_t rx_slot;
static volatile bool             packet_ready;

static volatile bool    nack_pending;
static volatile uint8_t nack_pending_reason;

static volatile bool    overrun_pending;

/* Frame-reception watchdog. Armed (set to FRAME_TIMEOUT_TICKS) when we
 * accept an SOF; decremented every TMR0 tick (~100 ms). If it reaches
 * zero before the frame completes, the state machine is forced back to
 * RX_IDLE so a truncated frame doesn't jam the receiver indefinitely.
 * Zero == disarmed (no frame in flight). */
#define FRAME_TIMEOUT_TICKS  5U   /* ~500 ms at 100 ms TMR0 cadence */
static volatile uint8_t frame_timeout_ticks;

static void on_overrun(void);
static void protocol_on_rx_byte(void);

void protocol_init(void) {
    rx_state = RX_IDLE;
    rx_payload_idx = 0;
    packet_ready = false;
    nack_pending = false;
    overrun_pending = false;
    frame_timeout_ticks = 0;

    /* Clear any latched OERR left by prior UART activity (the bootloader
     * uses the same EUSART for the firmware transfer and may leave the
     * receiver jammed at handoff). EUSART1_Initialize() writes RC1STA but
     * does not toggle CREN, so OERR survives. */
    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;

    EUSART1_RxCompleteCallbackRegister(protocol_on_rx_byte);
    EUSART1_OverrunErrorCallbackRegister(on_overrun);
}

void protocol_tick_isr(void) {
    /* Called from the TMR0 callback. If a frame has been mid-flight for
     * longer than FRAME_TIMEOUT_TICKS, force the state machine back to
     * IDLE so a truncated or glitched frame doesn't jam the receiver.
     * Disarmed when frame_timeout_ticks == 0. */
    if (frame_timeout_ticks > 0) {
        frame_timeout_ticks--;
        if (frame_timeout_ticks == 0) {
            rx_state = RX_IDLE;
            rx_payload_idx = 0;
        }
    }
}

static void on_overrun(void) {
    /* The MCC EUSART driver detects OERR but doesn't clear it; the receiver
     * stays jammed until CREN is toggled. Do that here so the link recovers
     * automatically. The errors_set() call is deferred to main context
     * (see protocol_take_pending_overrun) so the ISR stays minimal. */
    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;
    rx_state = RX_IDLE;
    rx_payload_idx = 0;
    frame_timeout_ticks = 0;
    overrun_pending = true;
}

static void protocol_on_rx_byte(void) {
    uint16_t received;
    uint8_t b;

    while (EUSART1_IsRxReady()) {
        b = EUSART1_Read();

        switch (rx_state) {
            case RX_IDLE:
                /* If main hasn't yet picked up the previous frame, drop the
                 * new one to avoid corrupting rx_slot mid-read. The host
                 * will time out and retry. */
                if (b == PROTOCOL_SOF && !packet_ready) {
                    rx_crc = CRC16_INIT;
                    rx_payload_idx = 0;
                    rx_state = RX_GOT_SOF;
                    frame_timeout_ticks = FRAME_TIMEOUT_TICKS;
                }
                break;

            case RX_GOT_SOF:
                rx_slot.opcode = b;
                rx_crc = crc16_update(rx_crc, b);
                rx_state = RX_GOT_OPCODE;
                break;

            case RX_GOT_OPCODE:
                if (b > PROTOCOL_MAX_PAYLOAD) {
                    rx_state = RX_IDLE;
                    frame_timeout_ticks = 0;
                    break;
                }
                rx_slot.len = b;
                rx_crc = crc16_update(rx_crc, b);
                if (b == 0) {
                    rx_state = RX_CRC_LO;
                } else {
                    rx_state = RX_PAYLOAD;
                }
                break;

            case RX_PAYLOAD:
                /* Write directly into the slot payload — no copy loop later.
                 * One byte per ISR entry; no loop here. */
                rx_slot.payload[rx_payload_idx] = b;
                rx_payload_idx++;
                rx_crc = crc16_update(rx_crc, b);
                if (rx_payload_idx >= rx_slot.len) {
                    rx_state = RX_CRC_LO;
                }
                break;

            case RX_CRC_LO:
                rx_crc_lo = b;
                rx_state = RX_CRC_HI;
                break;

            case RX_CRC_HI:
                received = (uint16_t)rx_crc_lo | ((uint16_t)b << 8);

                if (received == rx_crc) {
                    packet_ready = true;
                } else {
                    nack_pending = true;
                    nack_pending_reason = NACK_BAD_CRC;
                }

                rx_state = RX_IDLE;
                frame_timeout_ticks = 0;
                break;

            default:
                rx_state = RX_IDLE;
                break;
        }
    }
}

bool protocol_poll(protocol_frame_t *out) {
    bool got = false;
    INTCONbits.GIE = 0;
    if (packet_ready) {
        uint8_t i;
        uint8_t len = rx_slot.len;
        out->opcode = rx_slot.opcode;
        out->len    = len;
        for (i = 0; i < len; i++) {
            out->payload[i] = rx_slot.payload[i];
        }
        packet_ready = false;
        got = true;
    }
    INTCONbits.GIE = 1;
    return got;
}

bool protocol_take_pending_nack(uint8_t *reason) {
    bool got = false;
    INTCONbits.GIE = 0;
    if (nack_pending) {
        *reason = nack_pending_reason;
        nack_pending = false;
        got = true;
    }
    INTCONbits.GIE = 1;
    return got;
}

bool protocol_take_pending_overrun(void) {
    bool got = false;
    INTCONbits.GIE = 0;
    if (overrun_pending) {
        overrun_pending = false;
        got = true;
    }
    INTCONbits.GIE = 1;
    return got;
}

static bool write_byte(uint8_t b, bool blocking) {
    if (blocking) {
        while (!EUSART1_IsTxReady()) { }
        EUSART1_Write(b);
        return true;
    }
    if (!EUSART1_IsTxReady()) {
        return false;
    }
    EUSART1_Write(b);
    return true;
}

bool protocol_send(uint8_t opcode, const uint8_t *payload, uint8_t len, bool blocking) {
    uint8_t buf[PROTOCOL_MAX_FRAME_BYTES];
    uint16_t crc;
    uint8_t i;
    uint8_t total;

    if (len > PROTOCOL_MAX_PAYLOAD) {
        return false;
    }

    buf[0] = PROTOCOL_SOF;
    buf[1] = opcode;
    buf[2] = len;
    for (i = 0; i < len; i++) {
        buf[3 + i] = payload[i];
    }
    crc = crc16(&buf[1], (uint8_t)(2 + len));
    buf[3 + len]     = (uint8_t)(crc & 0xFFU);
    buf[3 + len + 1] = (uint8_t)((crc >> 8) & 0xFFU);
    total = (uint8_t)(4 + len + 1);  /* SOF + OP + LEN + payload + CRC_LO + CRC_HI */

    /* Non-blocking telemetry path: bail out before committing the first byte
     * if the TX FIFO is busy. Once we've committed the SOF we must finish
     * the frame -- the rest of the bytes use a bounded busy-wait. */
    if (!blocking && !EUSART1_IsTxReady()) {
        return false;
    }

    for (i = 0; i < total; i++) {
        (void)write_byte(buf[i], true);
    }
    return true;
}

void protocol_send_ack(uint8_t op_acked, const uint8_t *payload, uint8_t len) {
    uint8_t buf[PROTOCOL_MAX_PAYLOAD];
    uint8_t i;
    if (len + 1U > PROTOCOL_MAX_PAYLOAD) {
        len = PROTOCOL_MAX_PAYLOAD - 1U;
    }
    buf[0] = op_acked;
    for (i = 0; i < len; i++) {
        buf[1 + i] = payload[i];
    }
    (void)protocol_send(OP_ACK, buf, (uint8_t)(len + 1U), true);
}

void protocol_send_nack(uint8_t op_acked, uint8_t reason) {
    uint8_t buf[2];
    buf[0] = op_acked;
    buf[1] = reason;
    (void)protocol_send(OP_NACK, buf, 2, true);
}
