/*
 * telemetry.h - Subscribe/heartbeat fast-slow scheduler.
 *
 * When subscribed, push OP_TELEMETRY_PUSH every ~1 s.
 * When idle, push every ~60 s.
 * Subscribe is sticky until either OP_UNSUBSCRIBE or the heartbeat
 * watchdog times out (~10 s without OP_HEARTBEAT).
 *
 * Also flushes any pending error rising-edge as an OP_EVENT frame.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void telemetry_init(void);

void telemetry_on_subscribe(void);
void telemetry_on_unsubscribe(void);

/* ISR context: decrements heartbeat watchdog and the send-interval counter. */
void telemetry_tick_isr(void);

/* Main context: applies any subscribe expiry, sends scheduled telemetry,
 * and flushes pending error OP_EVENT. Call once per main-loop pass. */
void telemetry_run(void);

/* Build the telemetry payload in *out (must be >= 11 bytes) and return
 * the number of bytes written. Exposed for OP_GET_ALL. */
uint8_t telemetry_build_snapshot(uint8_t *out);

bool telemetry_is_subscribed(void);

/* Cached tank levels. RC7 is pulsed (via getWater/getWaste in power.c) only
 * once per telemetry push -- the cache is refreshed immediately before each
 * push, then re-used by errors_run_detectors and OP_GET_WATER/OP_GET_WASTE
 * for the rest of the cycle. Throttling reads this way protects the tank
 * sensor rods from electrochemical oxidation. */
uint8_t telemetry_get_water_cached(void);
uint8_t telemetry_get_waste_cached(void);

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_H */
