/*
 * errors.h - Sticky 16-bit error/warning flags with detectors.
 *
 * Firmware sets bits; only the host clears them (OP_CLEAR_ERRORS).
 * A rising edge (any bit going from 0 to 1) causes telemetry_run() to
 * push an OP_EVENT(kind=ERROR_RAISED) frame.
 */

#ifndef ERRORS_H
#define ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Catalogue. Match the order documented in the plan. */
/* SET_HOUSEHOLD 1 was issued but V_household never crossed the ON
 * threshold within ~30 s. Firmware also pulses OFF to disengage the
 * relay before latching this error. */
#define ERR_HOUSEHOLD_SWITCH_FAILED  0x0001U
#define ERR_VOLTAGE_HOUSEHOLD_LOW    0x0002U
#define ERR_VOLTAGE_STARTER_LOW      0x0004U
#define ERR_VOLTAGE_MAINS_LOW        0x0008U
#define ERR_WATER_LOW                0x0010U
#define ERR_WASTE_HIGH               0x0020U
#define ERR_WASTE_FULL               0x0040U
#define ERR_ADC_STUCK                0x0080U
#define ERR_PROTOCOL_CRC             0x0100U
#define ERR_PROTOCOL_OVERRUN         0x0200U
#define ERR_BROWN_OUT                0x0400U

void errors_init(void);

void errors_set(uint16_t mask);
void errors_clear(uint16_t mask);
uint16_t errors_get(void);

/* Returns the mask of bits that newly transitioned 0->1 since the last call,
 * and updates the "last reported" snapshot. Used by telemetry_run() to
 * emit one OP_EVENT per rising edge. */
uint16_t errors_take_rising_edge(void);

/* Arm/disarm the household switch watchdog. After arming, if the household
 * state does not reach ON within the watchdog interval, ERR_HOUSEHOLD_SWITCH_FAILED
 * is set. Called by the setHousehold(1) command handler. */
void errors_arm_household_switch(void);
void errors_disarm_household_switch(void);

/* Called from the main loop after adc_sampler_consume() to evaluate the
 * rule set against fresh values. */
void errors_run_detectors(void);

/* ISR-context tick: bumps the ADC-sampler stall counter. */
void errors_tick_isr(void);

/* Called from adc_sampler whenever a fresh cycle completes, to reset the
 * stall counter. */
void errors_note_adc_fresh(void);

#ifdef __cplusplus
}
#endif

#endif /* ERRORS_H */
