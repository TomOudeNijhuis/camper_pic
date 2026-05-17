#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "../power.h"
#include "../../mcc_generated_files/system/system.h"

#define POWER_PERIOD 10

const uint16_t FVRA4X = 0x811A;     // FVR1 @ 4x in [mV]
const uint16_t TSLR1 = 0x8112;      // TIM gain @ vdd=3v3, vref=2.048
const uint16_t TSLR3 = 0x8114;      // TIM offset @ vdd=3v3, vref=2.048

// Read a 14-bit word from configuration/DIA program memory (0x8000+).
// Replaces the MCC-generated CONFIGURATION_Read that was removed from this project.
static uint16_t configMemoryRead(uint16_t addr) {
    uint8_t gie = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    NVMADRL = (uint8_t)(addr & 0xFF);
    NVMADRH = (uint8_t)(addr >> 8);
    NVMCON1bits.NVMREGS = 1;
    NVMCON1bits.RD = 1;
    NOP();
    NOP();
    uint16_t result = ((uint16_t)NVMDATH << 8) | NVMDATL;
    INTCONbits.GIE = gie;
    return result;
}

// One-shot ADC conversion. The new ADC v2 driver no longer ships
// ADCC_GetSingleConversion, so this composes the three-step API.
static adc_result_t adcGetSingleConversion(adc_channel_t channel) {
    ADC_ChannelSelect(channel);
    ADC_ConversionStart();
    while (!ADC_IsConversionDone()) { }
    return ADC_ConversionResultGet();
}

const double RESISTOR_GAIN = (10000.0 + 3900.0) / 3900.0;

uint8_t householdMarker = 0;
uint16_t householdVoltage = 0;
uint16_t starterVoltage = 0;
uint16_t mainsVoltage = 0;
uint16_t temp = 0;
uint8_t counter = 0;

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

void convertVoltages(void) {
    uint16_t adc_result;
    uint16_t fvr1_value;
    double f;
    
    fvr1_value = configMemoryRead(FVRA4X);
    
    adc_result = adcGetSingleConversion(HOUSEHOLD);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    householdVoltage = (uint16_t)(f * RESISTOR_GAIN);
    
    adc_result = adcGetSingleConversion(STARTER);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    starterVoltage = (uint16_t)(f * RESISTOR_GAIN);

    adc_result = adcGetSingleConversion(MAINS);
    f = (double)adc_result * (double)fvr1_value / 1023.0;
    mainsVoltage = (uint16_t)(f * RESISTOR_GAIN);
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

const char * getPumpStateStr(void) {
    uint8_t i;
    
    i = getPumpState();
    
    if (i == 0)
        return "OFF";
    else if (i == 1)
        return "ON";
    
    return "Unknown state";
}

void setPump(uint8_t state) {
    if (state == 1) {
        PUMP_LAT = 1;
    } else {
        PUMP_LAT = 0;
    }
}

// The level switches are pulled up via the PULLUP pin (RC7) only while
// being read, then released. This avoids a hardware bug in steady-state.
static uint8_t readLevel(uint8_t s100, uint8_t s75, uint8_t s50, uint8_t s25) {
    if ((s100 == 0) && (s75 == 0) && (s50 == 0) && (s25 == 0)) {
        return 100;
    } else if ((s75 == 0) && (s50 == 0) && (s25 == 0)) {
        return 75;
    } else if ((s50 == 0) && (s25 == 0)) {
        return 50;
    } else if (s25 == 0) {
        return 25;
    } else {
        return 0;
    }
}

uint8_t getWaste() {
    uint8_t result;
    PULLUP_LAT = 1;
    __delay_ms(10);
    result = readLevel(WASTE100_PORT, WASTE75_PORT, WASTE50_PORT, WASTE25_PORT);
    PULLUP_LAT = 0;
    return result;
}

uint8_t getWater() {
    uint8_t result;
    PULLUP_LAT = 1;
    __delay_ms(10);
    result = readLevel(WATER100_PORT, WATER75_PORT, WATER50_PORT, WATER25_PORT);
    PULLUP_LAT = 0;
    return result;
}