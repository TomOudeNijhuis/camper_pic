/*
 * adc_sampler.h - Interrupt-chained ADC channel walker.
 *
 * TMR0 ISR triggers a cycle every ~500 ms via adc_sampler_start_cycle().
 * The ADC done callback (ISR) advances through HOUSEHOLD -> STARTER -> MAINS,
 * stashing raw counts. Main calls adc_sampler_consume() to apply the FVR/
 * resistor-gain math and update the cached mV per channel.
 *
 * No blocking polls; no floating-point in ISR.
 */

#ifndef ADC_SAMPLER_H
#define ADC_SAMPLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "power.h"

void adc_sampler_init(void);

/* Idempotent: starts a new sampling cycle only when the state machine is
 * IDLE. Safe from both ISR and main. */
void adc_sampler_start_cycle(void);

/* Main-context: if a fresh raw set is available, runs voltage math and
 * updates the cache. Returns true if the cache was refreshed. */
bool adc_sampler_consume(void);

uint16_t adc_sampler_get_cached(voltage_t ch);

#ifdef __cplusplus
}
#endif

#endif /* ADC_SAMPLER_H */
