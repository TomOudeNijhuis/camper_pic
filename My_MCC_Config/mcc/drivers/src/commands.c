#include <xc.h>
#include <string.h>
#include "../commands.h"
#include "../protocol.h"
#include "../power.h"
#include "../neopixels.h"
#include "../adc_sampler.h"
#include "../telemetry.h"
#include "../errors.h"
#include "../../mcc_generated_files/system/system.h"

#define BL_ENTRY_EEPROM_ADDR 0xF000U
#define BL_ENTRY_MARKER      0xA5U
#define BL_ENTRY_MAGIC       0x5AU   /* OP_ENTER_BOOTLOADER payload byte */

#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 0
#define FIRMWARE_VERSION_PATCH 0

/* Set the bootloader entry marker in EEPROM, then reset.
 * Kept byte-identical to the original main.c implementation so the
 * bootloader continues to recognise the rendezvous. */
static void enter_bootloader(void) {
    INTCONbits.GIE = 0;

    NVMCON1bits.NVMREGS = 1;
    NVMADRH = (uint8_t)(BL_ENTRY_EEPROM_ADDR >> 8);
    NVMADRL = (uint8_t)(BL_ENTRY_EEPROM_ADDR & 0xFFU);
    NVMDATL = BL_ENTRY_MARKER;
    NVMCON1bits.WREN = 1;
    NVMCON2 = 0x55;
    NVMCON2 = 0xAA;
    NVMCON1bits.WR = 1;
    while (NVMCON1bits.WR) { ; }
    NVMCON1bits.WREN = 0;

    __asm("reset");
}

static uint16_t le_u16(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static void put_u16_le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFFU);
    p[1] = (uint8_t)((v >> 8) & 0xFFU);
}

static void cmd_get_household(const protocol_frame_t *f) {
    uint8_t s = (uint8_t)getHouseholdState();
    protocol_send_ack(f->opcode, &s, 1);
}

static void cmd_get_pump(const protocol_frame_t *f) {
    uint8_t s = getPumpState();
    protocol_send_ack(f->opcode, &s, 1);
}

static void cmd_get_voltage(const protocol_frame_t *f) {
    uint8_t ch;
    uint16_t v;
    uint8_t out[3];

    if (f->len != 1) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    ch = f->payload[0];
    if (ch > V_STARTER) {
        protocol_send_nack(f->opcode, NACK_BAD_PARAM);
        return;
    }
    v = getVoltage((voltage_t)ch);
    out[0] = ch;
    put_u16_le(&out[1], v);
    protocol_send_ack(f->opcode, out, 3);
}

static void cmd_get_water(const protocol_frame_t *f) {
    uint8_t v = telemetry_get_water_cached();
    protocol_send_ack(f->opcode, &v, 1);
}

static void cmd_get_waste(const protocol_frame_t *f) {
    uint8_t v = telemetry_get_waste_cached();
    protocol_send_ack(f->opcode, &v, 1);
}

static void cmd_get_all(const protocol_frame_t *f) {
    uint8_t buf[16];
    uint8_t n = telemetry_build_snapshot(buf);
    protocol_send_ack(f->opcode, buf, n);
}

static void cmd_set_household(const protocol_frame_t *f) {
    if (f->len != 1) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    if (f->payload[0] > 1) {
        protocol_send_nack(f->opcode, NACK_BAD_PARAM);
        return;
    }
    setHousehold(f->payload[0]);
    if (f->payload[0] == 1) {
        errors_arm_household_switch();
    } else {
        errors_disarm_household_switch();
    }
    {
        uint8_t s = (uint8_t)getHouseholdState();
        protocol_send_ack(f->opcode, &s, 1);
    }
}

static void cmd_set_pump(const protocol_frame_t *f) {
    if (f->len != 1) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    if (f->payload[0] > 1) {
        protocol_send_nack(f->opcode, NACK_BAD_PARAM);
        return;
    }
    setPump(f->payload[0]);
    {
        uint8_t s = getPumpState();
        protocol_send_ack(f->opcode, &s, 1);
    }
}

#define NEOPIXEL_OVERRULE_TICKS 1800U   /* ~3 min at 100 ms TMR0 */

static void cmd_set_neopixel(const protocol_frame_t *f) {
    uint8_t led, color, duty;
    if (f->len != 3) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    led = f->payload[0];
    color = f->payload[1];
    duty = f->payload[2];
    if (led < 1 || led > 2 || color >= COLOR_COUNT || duty > 10) {
        protocol_send_nack(f->opcode, NACK_BAD_PARAM);
        return;
    }
    neopixel_setcolor(led, (ColorName)color, duty);
    neopixel_overrule(NEOPIXEL_OVERRULE_TICKS);
    protocol_send_ack(f->opcode, NULL, 0);
}

static void cmd_subscribe(const protocol_frame_t *f) {
    telemetry_on_subscribe();
    protocol_send_ack(f->opcode, NULL, 0);
}

static void cmd_unsubscribe(const protocol_frame_t *f) {
    telemetry_on_unsubscribe();
    protocol_send_ack(f->opcode, NULL, 0);
}

static void cmd_enter_bootloader(const protocol_frame_t *f) {
    if (f->len != 1) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    if (f->payload[0] != BL_ENTRY_MAGIC) {
        protocol_send_nack(f->opcode, NACK_BAD_PARAM);
        return;
    }
    protocol_send_ack(f->opcode, NULL, 0);
    __delay_ms(20);   /* let the ACK bytes drain out of the UART FIFO */
    enter_bootloader();
}

static void cmd_ping(const protocol_frame_t *f) {
    protocol_send_ack(f->opcode, f->payload, f->len);
}

static void cmd_version(const protocol_frame_t *f) {
    uint8_t v[3];
    v[0] = FIRMWARE_VERSION_MAJOR;
    v[1] = FIRMWARE_VERSION_MINOR;
    v[2] = FIRMWARE_VERSION_PATCH;
    protocol_send_ack(f->opcode, v, 3);
}

static void cmd_get_errors(const protocol_frame_t *f) {
    uint8_t out[2];
    put_u16_le(out, errors_get());
    protocol_send_ack(f->opcode, out, 2);
}

static void cmd_clear_errors(const protocol_frame_t *f) {
    uint8_t out[2];
    if (f->len != 2) {
        protocol_send_nack(f->opcode, NACK_BAD_LEN);
        return;
    }
    errors_clear(le_u16(f->payload));
    put_u16_le(out, errors_get());
    protocol_send_ack(f->opcode, out, 2);
}

void commands_dispatch(const protocol_frame_t *f) {
    switch (f->opcode) {
    case OP_GET_HOUSEHOLD:    cmd_get_household(f);    break;
    case OP_GET_PUMP:         cmd_get_pump(f);         break;
    case OP_GET_VOLTAGE:      cmd_get_voltage(f);      break;
    case OP_GET_WATER:        cmd_get_water(f);        break;
    case OP_GET_WASTE:        cmd_get_waste(f);        break;
    case OP_GET_ALL:          cmd_get_all(f);          break;
    case OP_SET_HOUSEHOLD:    cmd_set_household(f);    break;
    case OP_SET_PUMP:         cmd_set_pump(f);         break;
    case OP_SET_NEOPIXEL:     cmd_set_neopixel(f);     break;
    case OP_SUBSCRIBE:        cmd_subscribe(f);        break;
    case OP_UNSUBSCRIBE:      cmd_unsubscribe(f);      break;
    case OP_ENTER_BOOTLOADER: cmd_enter_bootloader(f); break;
    case OP_PING:             cmd_ping(f);             break;
    case OP_VERSION:          cmd_version(f);          break;
    case OP_GET_ERRORS:       cmd_get_errors(f);       break;
    case OP_CLEAR_ERRORS:     cmd_clear_errors(f);     break;
    default:
        protocol_send_nack(f->opcode, NACK_UNKNOWN_OPCODE);
        break;
    }
}
