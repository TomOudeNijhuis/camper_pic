#include <xc.h>
#include <string.h>
#include "../power.h"
#include "../adc_sampler.h"
#include "../../mcc_generated_files/system/system.h"

static uint8_t householdMarker = 0;

void powerInitialize(void) {
    ON_SetDigitalOutput();
    OFF_SetDigitalOutput();
    PUMP_SetDigitalOutput();
    PULLUP_SetDigitalOutput();

    ON_LAT = 0;
    OFF_LAT = 0;
    PUMP_LAT = 0;
    PULLUP_LAT = 0;
}

uint16_t getVoltage(voltage_t voltage) {
    return adc_sampler_get_cached(voltage);
}

household_state_t getHouseholdState(void) {
    return householdMarker ? ON : OFF;
}

const char * getHouseHoldStateStr(void) {
    return (getHouseholdState() == ON) ? "ON" : "OFF";
}

void setHousehold(uint8_t state) {
    householdMarker = state;

    if (state == 1) {
        ON_LAT = 1;
        __delay_ms(10);
        ON_LAT = 0;
    } else {
        OFF_LAT = 1;
        __delay_ms(10);
        OFF_LAT = 0;
    }
}

uint8_t getPumpState(void) {
    return PUMP_LAT;
}

const char * getPumpStateStr(void) {
    return (getPumpState() == 1) ? "ON" : "OFF";
}

void setPump(uint8_t state) {
    PUMP_LAT = (state == 1) ? 1 : 0;
}

/* The level switches are pulled up via the PULLUP pin (RC7) only while
 * being read, then released. This avoids a hardware bug in steady-state. */
static uint8_t readLevel(uint8_t s100, uint8_t s75, uint8_t s50, uint8_t s25) {
    if ((s100 == 0) && (s75 == 0) && (s50 == 0) && (s25 == 0)) {
        return 100;
    } else if ((s75 == 0) && (s50 == 0) && (s25 == 0)) {
        return 75;
    } else if ((s50 == 0) && (s25 == 0)) {
        return 50;
    } else if (s25 == 0) {
        return 25;
    }
    return 0;
}

uint8_t getWaste(void) {
    uint8_t result;
    PULLUP_LAT = 1;
    __delay_ms(50);
    result = readLevel(WASTE100_PORT, WASTE75_PORT, WASTE50_PORT, WASTE25_PORT);
    PULLUP_LAT = 0;
    return result;
}

uint8_t getWater(void) {
    uint8_t result;
    PULLUP_LAT = 1;
    __delay_ms(50);
    result = readLevel(WATER100_PORT, WATER75_PORT, WATER50_PORT, WATER25_PORT);
    PULLUP_LAT = 0;
    return result;
}
