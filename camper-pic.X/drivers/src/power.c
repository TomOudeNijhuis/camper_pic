#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "../power.h"
#include "../../mcc_generated_files/system/system.h"

#define POWER_PERIOD 10

const uint16_t FVRA4X = 0x811A;     // FVR1 @ 4x in [mV]
const uint16_t TSLR1 = 0x8112;      // TIM gain @ vdd=3v3, vref=2.048 
const uint16_t TSLR3 = 0x8114;      // TIM offset @ vdd=3v3, vref=2.048 

const double RESISTOR_GAIN = (10000.0 + 3900.0) / 3900.0;

uint8_t householdMarker = 0;
uint16_t householdVoltage = 0;
uint16_t starterVoltage = 0;
uint16_t mainsVoltage = 0;
uint16_t temp = 0;
uint8_t counter = 0;

void powerInitialize(void) {
    //ON_SetDigitalOutput();
    //OFF_SetDigitalOutput();
    //PUMP_SetDigitalOutput();
    
    //ON_LAT = 0;
    //OFF_LAT = 0;
    //PUMP_LAT = 0;
}

void convertVoltages(void) {
    uint16_t adc_result;
    uint16_t fvr1_value;
    double f;
    
    fvr1_value = CONFIGURATION_Read(FVRA4X);
    
    adc_result = ADCC_GetSingleConversion(HOUSEHOLD);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    householdVoltage = (uint16_t)(f * RESISTOR_GAIN);
    
    adc_result = ADCC_GetSingleConversion(STARTER);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    starterVoltage = (uint16_t)(f * RESISTOR_GAIN);

    adc_result = ADCC_GetSingleConversion(MAINS);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    mainsVoltage = (uint16_t)(f * RESISTOR_GAIN);

    /*
    printf( \
        "vfr1x4: %d, h: %d, s: %d, m: %d\r\n", \
        fvr1_value, householdVoltage, starterVoltage, mainsVoltage \
    );
    */
}

uint16_t getVoltage(voltage_t voltage) {
    if (voltage == V_HOUSEHOLD)
        return householdVoltage;
    else if (voltage == V_STARTER)
        return starterVoltage;
    else if (voltage == V_MAINS)
        return mainsVoltage;
    
    return 0;
}

void powerRunner(void) {
    if ((counter == 0) || (counter == 5)) {
        convertVoltages();
    } 
    
    if (counter < POWER_PERIOD) {
        counter++;
    } else
        counter = 0;
}

household_state_t getHouseholdState(void) {
    if (householdMarker == 0) {
        return OFF;
    } else if (householdVoltage > GEL_ACTIVE_TRESHOLD) {
        return ON;
    }

    return PENDING;
}

const char * getHouseHoldStateStr(void) {
    uint8_t i;
    
    i = getHouseholdState();
    
    if (i == 0)
        return "OFF";
    else if (i == 1)
        return "PENDING";
    else if (i == 2) 
        return "ON";
    
    return "Unknown state";
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

uint8_t getPumpState() {
    return PUMP_LAT;
}

void setPump(uint8_t state) {
    if (state == 1) {
        PUMP_LAT = 1;
    } else {
        PUMP_LAT = 0;
    }
}

uint8_t getWaste() {
    if ((WASTE100_PORT == 0) && (WASTE75_PORT == 0) && (WASTE50_PORT == 0) && (WASTE25_PORT == 0)) {
        return 100;
    } else if ((WASTE75_PORT == 0) && (WASTE50_PORT == 0) && (WASTE25_PORT == 0)) {
        return 75;
    } else if ((WASTE50_PORT == 0) && (WASTE25_PORT == 0)) {
        return 50;
    } else if (WASTE25_PORT == 0) {
        return 25;
    } else {
        return 0;
    }
}

uint8_t getWater() {
    if ((WATER100_PORT == 0) && (WATER75_PORT == 0) && (WATER50_PORT == 0) && (WATER25_PORT == 0)) {
        return 100;
    } else if ((WATER75_PORT == 0) && (WATER50_PORT == 0) && (WATER25_PORT == 0)) {
        return 75;
    } else if ((WATER50_PORT == 0) && (WATER25_PORT == 0)) {
        return 50;
    } else if (WATER25_PORT == 0) {
        return 25;
    } else {
        return 0;
    }
}