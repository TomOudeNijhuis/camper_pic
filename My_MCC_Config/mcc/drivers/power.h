/*
 * power.h - Relay, pump, and tank level abstractions.
 *
 * After the firmware upgrade, voltages are read from the adc_sampler cache,
 * not via blocking ADC conversions. getVoltage() is a one-liner that returns
 * the cached value last refreshed by adc_sampler_consume() in main.
 */

#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdint.h>

typedef enum {
    OFF = 0,
    ON = 1
} household_state_t;

typedef enum {
    V_HOUSEHOLD = 0,
    V_MAINS = 1,
    V_STARTER = 2
} voltage_t;

#define GEL_ACTIVE_TRESHOLD   9000U
#define GEL_ERROR_TRESHOLD   11800U
#define GEL_WARNING_TRESHOLD 12000U

void powerInitialize(void);

household_state_t getHouseholdState(void);
const char * getHouseHoldStateStr(void);
void setHousehold(uint8_t state);

uint8_t getPumpState(void);
const char * getPumpStateStr(void);
void setPump(uint8_t state);

uint16_t getVoltage(voltage_t voltage);
uint8_t getWater(void);
uint8_t getWaste(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_H */
