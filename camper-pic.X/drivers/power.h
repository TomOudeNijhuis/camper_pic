/* 
 * File:   power.h
 * Author: oudenijhuis
 *
 * Created on October 6, 2024, 7:57 PM
 */

#ifndef POWER_H
#define	POWER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
    
typedef enum {
    OFF = 0,
    PENDING = 1,
    ON = 2
} household_state_t;

typedef enum {
    V_HOUSEHOLD = 0,
    V_MAINS = 1,
    V_STARTER = 2
} voltage_t;

const uint16_t GEL_ACTIVE_TRESHOLD = 9000;
const uint16_t GEL_ERROR_TRESHOLD = 11800;
const uint16_t GEL_WARNING_TRESHOLD = 12000;

void powerInitialize(void);
void powerRunner(void);

household_state_t getHouseholdState(void);
const char * getHouseHoldStateStr(void);
void setHousehold(uint8_t state);

uint8_t getPumpState(void);
const char * getPumpStateStr(void);
void setPump(uint8_t state);

uint16_t getVoltage(voltage_t voltage);
uint8_t getWater(void);
uint8_t getWaste(void);

#ifdef	__cplusplus
}
#endif

#endif	/* POWER_H */

