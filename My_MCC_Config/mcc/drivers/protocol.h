/*
 * protocol.h - Binary frame layer over EUSART1.
 *
 * Frame: [SOF=0xAA][OPCODE][LEN][PAYLOAD...][CRC_LO][CRC_HI]
 * CRC over OPCODE+LEN+PAYLOAD using CRC-16/MCRF4XX (see crc16.h).
 * Max payload = 56 bytes (4 header + 56 + 2 CRC = 62 <= 64-byte EUSART1 FIFO).
 *
 * RX is driven from the EUSART1 RX-complete ISR. Main consumes completed
 * frames via protocol_poll(). TX is built in main context.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define PROTOCOL_SOF              0xAAU
#define PROTOCOL_MAX_PAYLOAD      56U
#define PROTOCOL_MAX_FRAME_BYTES  (4U + PROTOCOL_MAX_PAYLOAD + 2U)

typedef enum {
    /* Getters */
    OP_GET_HOUSEHOLD     = 0x10,
    OP_GET_PUMP          = 0x11,
    OP_GET_VOLTAGE       = 0x12,
    OP_GET_WATER         = 0x13,
    OP_GET_WASTE         = 0x14,
    OP_GET_ALL           = 0x15,
    /* Setters */
    OP_SET_HOUSEHOLD     = 0x20,
    OP_SET_PUMP          = 0x21,
    OP_SET_NEOPIXEL      = 0x22,
    /* Subscription / lifecycle */
    OP_SUBSCRIBE         = 0x30,
    /* 0x31 reserved (was OP_HEARTBEAT; SUBSCRIBE is now idempotent and
     * doubles as the keepalive) */
    OP_UNSUBSCRIBE       = 0x32,
    /* System */
    OP_ENTER_BOOTLOADER  = 0x40,
    OP_PING              = 0x41,
    OP_VERSION           = 0x42,
    OP_GET_ERRORS        = 0x43,
    OP_CLEAR_ERRORS      = 0x44,
    /* Async pushes */
    OP_TELEMETRY_PUSH    = 0x80,
    OP_EVENT             = 0x81,
    /* Replies */
    OP_ACK               = 0xF0,
    OP_NACK              = 0xF1
} opcode_t;

typedef enum {
    NACK_BAD_CRC        = 0x01,
    NACK_BAD_LEN        = 0x02,
    NACK_UNKNOWN_OPCODE = 0x03,
    NACK_BAD_PARAM      = 0x04,
    NACK_BUSY           = 0x05
} nack_reason_t;

typedef enum {
    EVENT_ERROR_RAISED = 0x01,
    EVENT_DEBUG        = 0x02   /* RX byte/frame/CRC-fail counters */
} event_kind_t;

typedef struct {
    uint8_t opcode;
    uint8_t len;
    uint8_t payload[PROTOCOL_MAX_PAYLOAD];
} protocol_frame_t;

void protocol_init(void);

bool protocol_poll(protocol_frame_t *out);

bool protocol_send(uint8_t opcode, const uint8_t *payload, uint8_t len, bool blocking);

void protocol_send_ack(uint8_t op_acked, const uint8_t *payload, uint8_t len);

void protocol_send_nack(uint8_t op_acked, uint8_t reason);

/* Returns and clears a pending NACK from a CRC-failed frame, if any.
 * Called by main to optionally emit a NACK with opcode=0xFF
 * (we don't know which opcode it was -- the frame was corrupt). */
bool protocol_take_pending_nack(uint8_t *reason);

/* Read and clear the RX debug counters. ISR-safe. */
void protocol_get_debug_counts(uint16_t *bytes, uint16_t *frames, uint16_t *crc_fails);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */
