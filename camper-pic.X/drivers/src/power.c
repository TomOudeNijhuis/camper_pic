#include "../power.h"
#include "../../mcc_generated_files/system/system.h"

union power_state {
    uint8_t as_byte;
    struct {
        unsigned household_marker : 1;
        unsigned household_status : 1;
    };
};

const uint16_t FVRA4X = 0x811A;     // FVR1 @ 4x in [mV]
const uint16_t TSLR1 = 0x8112;      // TIM gain @ vdd=3v3, vref=2.048 
const uint16_t TSLR3 = 0x8114;      // TIM offset @ vdd=3v3, vref=2.048 

const double RESISTOR_GAIN = (10000.0 + 3900.0) / 3900.0;

uint16_t householdVoltage = 0;
uint16_t starterVoltage = 0;
uint16_t mainsVoltage = 0;
uint16_t temp = 0;

void convert_powers(void) {
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
    
    adc_result = ADCC_GetSingleConversion(channel_temp);
    temp = adc_result;
    
    printf( \
        "vfr1x4: %d, h: %d, s: %f, m: %d, t: %d\r\n", \
        fvr1_value, householdVoltage, starterVoltage, mainsVoltage, temp \
    );
}