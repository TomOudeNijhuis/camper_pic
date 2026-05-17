#include <xc.h>
#include <string.h>
#include "../protocol.h"
#include "../crc16.h"
#include "../errors.h"
#include "../../mcc_generated_files/uart/eusart1.h"

typedef enum {
    RX_IDLE = 0,
    RX_GOT_SOF,
    RX_GOT_OPCODE,
    RX_GOT_LEN,
    RX_PAYLOAD,
    RX_CRC_LO,
    RX_CRC_HI
} rx_state_t;

static volatile rx_state_t rx_state = RX_IDLE;
static volatile uint16_t   rx_crc;
static volatile uint8_t    rx_opcode;
static volatile uint8_t    rx_len;
static volatile uint8_t    rx_payload[PROTOCOL_MAX_PAYLOAD];
static volatile uint8_t    rx_payload_idx;
static volatile uint8_t    rx_crc_lo;

static volatile protocol_frame_t rx_slot[2];
static volatile uint8_t          rx_slot_active;   /* slot index ready for main */
static volatile bool             packet_ready;

static volatile bool    nack_pending;
static volatile uint8_t nack_pending_reason;

/* Debug counters exposed via OP_DEBUG. Cleared on read. */
static volatile uint16_t rx_byte_count;
static volatile uint16_t rx_frame_count;
static volatile uint16_t rx_crc_fail_count;

static void on_overrun(void);
static void protocol_on_rx_byte(void);

void protocol_init(void) {
    rx_state = RX_IDLE;
    rx_payload_idx = 0;
    packet_ready = false;
    nack_pending = false;
    rx_slot_active = 0;

    /* Clear any latched OERR left by prior UART activity (the bootloader
     * uses the same EUSART for the firmware transfer and may leave the
     * receiver jammed at handoff). EUSART1_Initialize() writes RC1STA but
     * does not toggle CREN, so OERR survives. */
    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;

    EUSART1_RxCompleteCallbackRegister(protocol_on_rx_byte);
    EUSART1_OverrunErrorCallbackRegister(on_overrun);
}

static void on_overrun(void) {
    /* The MCC EUSART driver detects OERR but doesn't clear it; the receiver
     * stays jammed until CREN is toggled. Do that here so the link recovers
     * automatically. */
    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;
    rx_state = RX_IDLE;
    rx_payload_idx = 0;
    errors_set(ERR_PROTOCOL_OVERRUN);
}

static void feed_byte(uint8_t b) {
    uint16_t received;

    switch (rx_state) {
        case RX_IDLE:
            if (b == PROTOCOL_SOF) {
                rx_crc = CRC16_INIT;
                rx_payload_idx = 0;
                rx_state = RX_GOT_SOF;
            }
            break;

        case RX_GOT_SOF:
            rx_opcode = b;
            rx_crc = crc16_update(rx_crc, b);
            rx_state = RX_GOT_OPCODE;
            break;

        case RX_GOT_OPCODE:
            if (b > PROTOCOL_MAX_PAYLOAD) {
                rx_state = RX_IDLE;
                break;
            }
            rx_len = b;
            rx_crc = crc16_update(rx_crc, b);
            if (rx_len == 0) {
                rx_state = RX_CRC_LO;
            } else {
                rx_state = RX_PAYLOAD;
            }
            break;

        case RX_PAYLOAD:
            rx_payload[rx_payload_idx++] = b;
            rx_crc = crc16_update(rx_crc, b);
            if (rx_payload_idx >= rx_len) {
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
                uint8_t slot = (uint8_t)(rx_slot_active ^ 1);
                uint8_t i;
                rx_slot[slot].opcode = rx_opcode;
                rx_slot[slot].len = rx_len;
                for (i = 0; i < rx_len; i++) {
                    rx_slot[slot].payload[i] = rx_payload[i];
                }
                rx_slot_active = slot;
                packet_ready = true;
                rx_frame_count++;
            } else {
                nack_pending = true;
                nack_pending_reason = NACK_BAD_CRC;
                errors_set(ERR_PROTOCOL_CRC);
                rx_crc_fail_count++;
            }
            rx_state = RX_IDLE;
            break;

        default:
            rx_state = RX_IDLE;
            break;
    }
}

/* ISR context: drain whatever is in the EUSART1 RX FIFO. */
static void protocol_on_rx_byte(void) {
    while (EUSART1_IsRxReady()) {
        uint8_t b = EUSART1_Read();
        rx_byte_count++;
        feed_byte(b);
    }
}

void protocol_get_debug_counts(uint16_t *bytes, uint16_t *frames, uint16_t *crc_fails) {
    /* Cumulative; do NOT clear on read so the host can spot any change
     * by simply watching the numbers tick up. */
    INTCONbits.GIE = 0;
    *bytes     = rx_byte_count;
    *frames    = rx_frame_count;
    *crc_fails = rx_crc_fail_count;
    INTCONbits.GIE = 1;
}

bool protocol_poll(protocol_frame_t *out) {
    bool got = false;
    INTCONbits.GIE = 0;
    if (packet_ready) {
        uint8_t slot = rx_slot_active;
        uint8_t i;
        out->opcode = rx_slot[slot].opcode;
        out->len    = rx_slot[slot].len;
        for (i = 0; i < out->len; i++) {
            out->payload[i] = rx_slot[slot].payload[i];
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
