#include <xc.h>
#include <string.h>
#include "../telemetry.h"
#include "../protocol.h"
#include "../power.h"
#include "../adc_sampler.h"
#include "../errors.h"
#include "../../mcc_generated_files/system/pins.h"

#define HEARTBEAT_TIMEOUT_TICKS      100U   /* 10 s @ 100 ms TMR0 */
#define SUBSCRIBED_FAST_INTERVAL      10U   /* 1 s */
#define IDLE_SLOW_INTERVAL           600U   /* 60 s */

#define TELEMETRY_PAYLOAD_LEN         11U

#define STATE_FLAG_HOUSEHOLD_ON        0x01U
/* bit 1 reserved (was HOUSEHOLD_PENDING; PENDING state removed) */
#define STATE_FLAG_PUMP_ON             0x04U
#define STATE_FLAG_SWITCH1             0x08U
#define STATE_FLAG_SWITCH2             0x10U
#define STATE_FLAG_VALUES_CHANGED      0x20U
#define STATE_FLAG_ERRORS_ACTIVE       0x40U
#define STATE_FLAG_SUBSCRIBED          0x80U

static volatile bool     subscribed;
static volatile uint16_t heartbeat_watchdog_ticks;
static volatile uint16_t next_send_ticks;
static volatile bool     sub_expired_pending;

static uint8_t last_pushed_payload[TELEMETRY_PAYLOAD_LEN];
static bool    last_payload_valid;

/* Tank levels are only sampled once per telemetry push, to limit RC7
 * (PULLUP) activity and slow electrochemical wear on the tank rods. The
 * cache is seeded to a benign "tank full / waste empty" so detectors
 * don't false-trigger before the first push happens. */
static uint8_t cached_water = 100;
static uint8_t cached_waste = 0;

uint8_t telemetry_get_water_cached(void) {
    return cached_water;
}

uint8_t telemetry_get_waste_cached(void) {
    return cached_waste;
}

static void put_u16_le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFFU);
    p[1] = (uint8_t)((v >> 8) & 0xFFU);
}

void telemetry_init(void) {
    subscribed = false;
    heartbeat_watchdog_ticks = 0;
    next_send_ticks = 0;
    sub_expired_pending = false;
    last_payload_valid = false;
    cached_water = 100;
    cached_waste = 0;
}

void telemetry_on_subscribe(void) {
    /* Idempotent: each call refreshes the heartbeat watchdog. The fast-
     * streaming cadence is only restarted from zero on the rising edge
     * (false -> true), so a host calling this every ~3 s as a keepalive
     * doesn't force an extra immediate frame. */
    INTCONbits.GIE = 0;
    if (!subscribed) {
        subscribed = true;
        next_send_ticks = 0;       /* first frame goes out immediately */
    }
    heartbeat_watchdog_ticks = HEARTBEAT_TIMEOUT_TICKS;
    sub_expired_pending = false;
    INTCONbits.GIE = 1;
}

void telemetry_on_unsubscribe(void) {
    INTCONbits.GIE = 0;
    subscribed = false;
    next_send_ticks = IDLE_SLOW_INTERVAL;
    INTCONbits.GIE = 1;
}

void telemetry_tick_isr(void) {
    if (subscribed && heartbeat_watchdog_ticks > 0) {
        heartbeat_watchdog_ticks--;
        if (heartbeat_watchdog_ticks == 0) {
            sub_expired_pending = true;
        }
    }
    if (next_send_ticks > 0) {
        next_send_ticks--;
    }
}

bool telemetry_is_subscribed(void) {
    return subscribed;
}

uint8_t telemetry_build_snapshot(uint8_t *out) {
    uint16_t v_household = getVoltage(V_HOUSEHOLD);
    uint16_t v_mains     = getVoltage(V_MAINS);
    uint16_t v_starter   = getVoltage(V_STARTER);
    uint8_t  water       = cached_water;
    uint8_t  waste       = cached_waste;
    household_state_t hh = getHouseholdState();
    uint8_t  pump        = getPumpState();
    uint16_t errs        = errors_get();
    uint8_t  flags       = 0;

    if (hh == ON)      flags |= STATE_FLAG_HOUSEHOLD_ON;
    if (pump)          flags |= STATE_FLAG_PUMP_ON;
    if (SWITCH1_PORT)  flags |= STATE_FLAG_SWITCH1;
    if (SWITCH2_PORT)  flags |= STATE_FLAG_SWITCH2;
    if (errs)          flags |= STATE_FLAG_ERRORS_ACTIVE;
    if (subscribed)    flags |= STATE_FLAG_SUBSCRIBED;

    put_u16_le(&out[0], v_household);
    put_u16_le(&out[2], v_mains);
    put_u16_le(&out[4], v_starter);
    out[6] = water;
    out[7] = waste;
    out[8] = flags;
    put_u16_le(&out[9], errs);
    return TELEMETRY_PAYLOAD_LEN;
}

static void flush_error_event(void) {
    uint16_t rising = errors_take_rising_edge();
    if (rising != 0) {
        uint8_t buf[3];
        buf[0] = EVENT_ERROR_RAISED;
        put_u16_le(&buf[1], rising);
        (void)protocol_send(OP_EVENT, buf, 3, true);
    }
}

void telemetry_run(void) {
    bool expired;
    INTCONbits.GIE = 0;
    expired = sub_expired_pending;
    sub_expired_pending = false;
    INTCONbits.GIE = 1;

    if (expired) {
        INTCONbits.GIE = 0;
        subscribed = false;
        next_send_ticks = IDLE_SLOW_INTERVAL;
        INTCONbits.GIE = 1;
    }

    flush_error_event();

    if (next_send_ticks == 0) {
        uint8_t payload[TELEMETRY_PAYLOAD_LEN];
        uint8_t i;
        bool changed = false;

        (void)adc_sampler_consume();
        /* Refresh tank cache once per push -- the only place RC7 is pulsed.
         * Everyone else (errors, OP_GET_WATER/WASTE, snapshot) reads from
         * the cache. */
        cached_water = getWater();
        cached_waste = getWaste();
        telemetry_build_snapshot(payload);

        if (last_payload_valid) {
            for (i = 0; i < TELEMETRY_PAYLOAD_LEN; i++) {
                if (payload[i] != last_pushed_payload[i]) {
                    changed = true;
                    break;
                }
            }
        } else {
            changed = true;
        }

        if (changed) {
            payload[8] |= STATE_FLAG_VALUES_CHANGED;
        }

        if (protocol_send(OP_TELEMETRY_PUSH, payload, TELEMETRY_PAYLOAD_LEN, false)) {
            /* Stash WITHOUT the VALUES_CHANGED bit so the next comparison
             * detects real field changes rather than seeing this self-set
             * bit toggle on every push. */
            for (i = 0; i < TELEMETRY_PAYLOAD_LEN; i++) {
                last_pushed_payload[i] = payload[i];
            }
            last_pushed_payload[8] &= (uint8_t)~STATE_FLAG_VALUES_CHANGED;
            last_payload_valid = true;
        }

        INTCONbits.GIE = 0;
        next_send_ticks = subscribed ? SUBSCRIBED_FAST_INTERVAL : IDLE_SLOW_INTERVAL;
        INTCONbits.GIE = 1;
    }
}
