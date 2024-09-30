/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  3.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef PINS_H
#define PINS_H

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_RA0 aliases
#define WASTE100_TRIS                 TRISAbits.TRISA0
#define WASTE100_LAT                  LATAbits.LATA0
#define WASTE100_PORT                 PORTAbits.RA0
#define WASTE100_WPU                  WPUAbits.WPUA0
#define WASTE100_OD                   ODCONAbits.ODCA0
#define WASTE100_ANS                  ANSELAbits.ANSA0
#define WASTE100_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define WASTE100_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define WASTE100_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define WASTE100_GetValue()           PORTAbits.RA0
#define WASTE100_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define WASTE100_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define WASTE100_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define WASTE100_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define WASTE100_SetPushPull()        do { ODCONAbits.ODCA0 = 0; } while(0)
#define WASTE100_SetOpenDrain()       do { ODCONAbits.ODCA0 = 1; } while(0)
#define WASTE100_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define WASTE100_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)
// get/set IO_RA1 aliases
#define WASTE75_TRIS                 TRISAbits.TRISA1
#define WASTE75_LAT                  LATAbits.LATA1
#define WASTE75_PORT                 PORTAbits.RA1
#define WASTE75_WPU                  WPUAbits.WPUA1
#define WASTE75_OD                   ODCONAbits.ODCA1
#define WASTE75_ANS                  ANSELAbits.ANSA1
#define WASTE75_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define WASTE75_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define WASTE75_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define WASTE75_GetValue()           PORTAbits.RA1
#define WASTE75_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define WASTE75_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define WASTE75_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define WASTE75_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define WASTE75_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define WASTE75_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define WASTE75_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define WASTE75_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)
// get/set IO_RA2 aliases
#define WASTE50_TRIS                 TRISAbits.TRISA2
#define WASTE50_LAT                  LATAbits.LATA2
#define WASTE50_PORT                 PORTAbits.RA2
#define WASTE50_WPU                  WPUAbits.WPUA2
#define WASTE50_OD                   ODCONAbits.ODCA2
#define WASTE50_ANS                  ANSELAbits.ANSA2
#define WASTE50_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define WASTE50_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define WASTE50_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define WASTE50_GetValue()           PORTAbits.RA2
#define WASTE50_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define WASTE50_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define WASTE50_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define WASTE50_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define WASTE50_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define WASTE50_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define WASTE50_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define WASTE50_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)
// get/set IO_RA3 aliases
#define WASTE25_TRIS                 TRISAbits.TRISA3
#define WASTE25_LAT                  LATAbits.LATA3
#define WASTE25_PORT                 PORTAbits.RA3
#define WASTE25_WPU                  WPUAbits.WPUA3
#define WASTE25_OD                   ODCONAbits.ODCA3
#define WASTE25_ANS                  ANSELAbits.ANSA3
#define WASTE25_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define WASTE25_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define WASTE25_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define WASTE25_GetValue()           PORTAbits.RA3
#define WASTE25_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define WASTE25_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define WASTE25_SetPullup()          do { WPUAbits.WPUA3 = 1; } while(0)
#define WASTE25_ResetPullup()        do { WPUAbits.WPUA3 = 0; } while(0)
#define WASTE25_SetPushPull()        do { ODCONAbits.ODCA3 = 0; } while(0)
#define WASTE25_SetOpenDrain()       do { ODCONAbits.ODCA3 = 1; } while(0)
#define WASTE25_SetAnalogMode()      do { ANSELAbits.ANSA3 = 1; } while(0)
#define WASTE25_SetDigitalMode()     do { ANSELAbits.ANSA3 = 0; } while(0)
// get/set IO_RA4 aliases
#define WATER100_TRIS                 TRISAbits.TRISA4
#define WATER100_LAT                  LATAbits.LATA4
#define WATER100_PORT                 PORTAbits.RA4
#define WATER100_WPU                  WPUAbits.WPUA4
#define WATER100_OD                   ODCONAbits.ODCA4
#define WATER100_ANS                  ANSELAbits.ANSA4
#define WATER100_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define WATER100_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define WATER100_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define WATER100_GetValue()           PORTAbits.RA4
#define WATER100_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define WATER100_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define WATER100_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define WATER100_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define WATER100_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define WATER100_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define WATER100_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define WATER100_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)
// get/set IO_RA5 aliases
#define WATER75_TRIS                 TRISAbits.TRISA5
#define WATER75_LAT                  LATAbits.LATA5
#define WATER75_PORT                 PORTAbits.RA5
#define WATER75_WPU                  WPUAbits.WPUA5
#define WATER75_OD                   ODCONAbits.ODCA5
#define WATER75_ANS                  ANSELAbits.ANSA5
#define WATER75_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define WATER75_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define WATER75_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define WATER75_GetValue()           PORTAbits.RA5
#define WATER75_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define WATER75_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define WATER75_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define WATER75_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define WATER75_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define WATER75_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define WATER75_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define WATER75_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)
// get/set IO_RA6 aliases
#define WATER25_TRIS                 TRISAbits.TRISA6
#define WATER25_LAT                  LATAbits.LATA6
#define WATER25_PORT                 PORTAbits.RA6
#define WATER25_WPU                  WPUAbits.WPUA6
#define WATER25_OD                   ODCONAbits.ODCA6
#define WATER25_ANS                  ANSELAbits.ANSA6
#define WATER25_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define WATER25_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define WATER25_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define WATER25_GetValue()           PORTAbits.RA6
#define WATER25_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define WATER25_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)
#define WATER25_SetPullup()          do { WPUAbits.WPUA6 = 1; } while(0)
#define WATER25_ResetPullup()        do { WPUAbits.WPUA6 = 0; } while(0)
#define WATER25_SetPushPull()        do { ODCONAbits.ODCA6 = 0; } while(0)
#define WATER25_SetOpenDrain()       do { ODCONAbits.ODCA6 = 1; } while(0)
#define WATER25_SetAnalogMode()      do { ANSELAbits.ANSA6 = 1; } while(0)
#define WATER25_SetDigitalMode()     do { ANSELAbits.ANSA6 = 0; } while(0)
// get/set IO_RA7 aliases
#define WATER50_TRIS                 TRISAbits.TRISA7
#define WATER50_LAT                  LATAbits.LATA7
#define WATER50_PORT                 PORTAbits.RA7
#define WATER50_WPU                  WPUAbits.WPUA7
#define WATER50_OD                   ODCONAbits.ODCA7
#define WATER50_ANS                  ANSELAbits.ANSA7
#define WATER50_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define WATER50_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define WATER50_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define WATER50_GetValue()           PORTAbits.RA7
#define WATER50_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define WATER50_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)
#define WATER50_SetPullup()          do { WPUAbits.WPUA7 = 1; } while(0)
#define WATER50_ResetPullup()        do { WPUAbits.WPUA7 = 0; } while(0)
#define WATER50_SetPushPull()        do { ODCONAbits.ODCA7 = 0; } while(0)
#define WATER50_SetOpenDrain()       do { ODCONAbits.ODCA7 = 1; } while(0)
#define WATER50_SetAnalogMode()      do { ANSELAbits.ANSA7 = 1; } while(0)
#define WATER50_SetDigitalMode()     do { ANSELAbits.ANSA7 = 0; } while(0)
// get/set IO_RB0 aliases
#define OFF_TRIS                 TRISBbits.TRISB0
#define OFF_LAT                  LATBbits.LATB0
#define OFF_PORT                 PORTBbits.RB0
#define OFF_WPU                  WPUBbits.WPUB0
#define OFF_OD                   ODCONBbits.ODCB0
#define OFF_ANS                  ANSELBbits.ANSB0
#define OFF_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define OFF_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define OFF_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define OFF_GetValue()           PORTBbits.RB0
#define OFF_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define OFF_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define OFF_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define OFF_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define OFF_SetPushPull()        do { ODCONBbits.ODCB0 = 0; } while(0)
#define OFF_SetOpenDrain()       do { ODCONBbits.ODCB0 = 1; } while(0)
#define OFF_SetAnalogMode()      do { ANSELBbits.ANSB0 = 1; } while(0)
#define OFF_SetDigitalMode()     do { ANSELBbits.ANSB0 = 0; } while(0)
// get/set IO_RB1 aliases
#define ON_TRIS                 TRISBbits.TRISB1
#define ON_LAT                  LATBbits.LATB1
#define ON_PORT                 PORTBbits.RB1
#define ON_WPU                  WPUBbits.WPUB1
#define ON_OD                   ODCONBbits.ODCB1
#define ON_ANS                  ANSELBbits.ANSB1
#define ON_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define ON_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define ON_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define ON_GetValue()           PORTBbits.RB1
#define ON_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define ON_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define ON_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define ON_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define ON_SetPushPull()        do { ODCONBbits.ODCB1 = 0; } while(0)
#define ON_SetOpenDrain()       do { ODCONBbits.ODCB1 = 1; } while(0)
#define ON_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define ON_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)
// get/set IO_RB2 aliases
#define HOUSEHOLD_TRIS                 TRISBbits.TRISB2
#define HOUSEHOLD_LAT                  LATBbits.LATB2
#define HOUSEHOLD_PORT                 PORTBbits.RB2
#define HOUSEHOLD_WPU                  WPUBbits.WPUB2
#define HOUSEHOLD_OD                   ODCONBbits.ODCB2
#define HOUSEHOLD_ANS                  ANSELBbits.ANSB2
#define HOUSEHOLD_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define HOUSEHOLD_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define HOUSEHOLD_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define HOUSEHOLD_GetValue()           PORTBbits.RB2
#define HOUSEHOLD_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define HOUSEHOLD_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define HOUSEHOLD_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define HOUSEHOLD_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define HOUSEHOLD_SetPushPull()        do { ODCONBbits.ODCB2 = 0; } while(0)
#define HOUSEHOLD_SetOpenDrain()       do { ODCONBbits.ODCB2 = 1; } while(0)
#define HOUSEHOLD_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define HOUSEHOLD_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)
// get/set IO_RB3 aliases
#define MAINS_TRIS                 TRISBbits.TRISB3
#define MAINS_LAT                  LATBbits.LATB3
#define MAINS_PORT                 PORTBbits.RB3
#define MAINS_WPU                  WPUBbits.WPUB3
#define MAINS_OD                   ODCONBbits.ODCB3
#define MAINS_ANS                  ANSELBbits.ANSB3
#define MAINS_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define MAINS_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define MAINS_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define MAINS_GetValue()           PORTBbits.RB3
#define MAINS_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define MAINS_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define MAINS_SetPullup()          do { WPUBbits.WPUB3 = 1; } while(0)
#define MAINS_ResetPullup()        do { WPUBbits.WPUB3 = 0; } while(0)
#define MAINS_SetPushPull()        do { ODCONBbits.ODCB3 = 0; } while(0)
#define MAINS_SetOpenDrain()       do { ODCONBbits.ODCB3 = 1; } while(0)
#define MAINS_SetAnalogMode()      do { ANSELBbits.ANSB3 = 1; } while(0)
#define MAINS_SetDigitalMode()     do { ANSELBbits.ANSB3 = 0; } while(0)
// get/set IO_RB4 aliases
#define STARTER_TRIS                 TRISBbits.TRISB4
#define STARTER_LAT                  LATBbits.LATB4
#define STARTER_PORT                 PORTBbits.RB4
#define STARTER_WPU                  WPUBbits.WPUB4
#define STARTER_OD                   ODCONBbits.ODCB4
#define STARTER_ANS                  ANSELBbits.ANSB4
#define STARTER_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define STARTER_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define STARTER_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define STARTER_GetValue()           PORTBbits.RB4
#define STARTER_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define STARTER_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define STARTER_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define STARTER_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define STARTER_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define STARTER_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define STARTER_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define STARTER_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)
// get/set IO_RB5 aliases
#define PUMP_TRIS                 TRISBbits.TRISB5
#define PUMP_LAT                  LATBbits.LATB5
#define PUMP_PORT                 PORTBbits.RB5
#define PUMP_WPU                  WPUBbits.WPUB5
#define PUMP_OD                   ODCONBbits.ODCB5
#define PUMP_ANS                  ANSELBbits.ANSB5
#define PUMP_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define PUMP_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define PUMP_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define PUMP_GetValue()           PORTBbits.RB5
#define PUMP_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define PUMP_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define PUMP_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define PUMP_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define PUMP_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define PUMP_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define PUMP_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define PUMP_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)
// get/set IO_RB6 aliases
#define SWITCH1_TRIS                 TRISBbits.TRISB6
#define SWITCH1_LAT                  LATBbits.LATB6
#define SWITCH1_PORT                 PORTBbits.RB6
#define SWITCH1_WPU                  WPUBbits.WPUB6
#define SWITCH1_OD                   ODCONBbits.ODCB6
#define SWITCH1_ANS                  ANSELBbits.ANSB6
#define SWITCH1_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define SWITCH1_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define SWITCH1_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define SWITCH1_GetValue()           PORTBbits.RB6
#define SWITCH1_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define SWITCH1_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define SWITCH1_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define SWITCH1_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)
#define SWITCH1_SetPushPull()        do { ODCONBbits.ODCB6 = 0; } while(0)
#define SWITCH1_SetOpenDrain()       do { ODCONBbits.ODCB6 = 1; } while(0)
#define SWITCH1_SetAnalogMode()      do { ANSELBbits.ANSB6 = 1; } while(0)
#define SWITCH1_SetDigitalMode()     do { ANSELBbits.ANSB6 = 0; } while(0)
#define RB6_SetInterruptHandler  SWITCH1_SetInterruptHandler
// get/set IO_RB7 aliases
#define SWITCH2_TRIS                 TRISBbits.TRISB7
#define SWITCH2_LAT                  LATBbits.LATB7
#define SWITCH2_PORT                 PORTBbits.RB7
#define SWITCH2_WPU                  WPUBbits.WPUB7
#define SWITCH2_OD                   ODCONBbits.ODCB7
#define SWITCH2_ANS                  ANSELBbits.ANSB7
#define SWITCH2_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define SWITCH2_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define SWITCH2_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define SWITCH2_GetValue()           PORTBbits.RB7
#define SWITCH2_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define SWITCH2_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define SWITCH2_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define SWITCH2_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)
#define SWITCH2_SetPushPull()        do { ODCONBbits.ODCB7 = 0; } while(0)
#define SWITCH2_SetOpenDrain()       do { ODCONBbits.ODCB7 = 1; } while(0)
#define SWITCH2_SetAnalogMode()      do { ANSELBbits.ANSB7 = 1; } while(0)
#define SWITCH2_SetDigitalMode()     do { ANSELBbits.ANSB7 = 0; } while(0)
#define RB7_SetInterruptHandler  SWITCH2_SetInterruptHandler
// get/set IO_RC2 aliases
#define NEOPIXEL_TRIS                 TRISCbits.TRISC2
#define NEOPIXEL_LAT                  LATCbits.LATC2
#define NEOPIXEL_PORT                 PORTCbits.RC2
#define NEOPIXEL_WPU                  WPUCbits.WPUC2
#define NEOPIXEL_OD                   ODCONCbits.ODCC2
#define NEOPIXEL_ANS                  ANSELCbits.ANSC2
#define NEOPIXEL_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define NEOPIXEL_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define NEOPIXEL_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define NEOPIXEL_GetValue()           PORTCbits.RC2
#define NEOPIXEL_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define NEOPIXEL_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define NEOPIXEL_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define NEOPIXEL_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define NEOPIXEL_SetPushPull()        do { ODCONCbits.ODCC2 = 0; } while(0)
#define NEOPIXEL_SetOpenDrain()       do { ODCONCbits.ODCC2 = 1; } while(0)
#define NEOPIXEL_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define NEOPIXEL_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)
// get/set IO_RC3 aliases
#define UART_RX_TRIS                 TRISCbits.TRISC3
#define UART_RX_LAT                  LATCbits.LATC3
#define UART_RX_PORT                 PORTCbits.RC3
#define UART_RX_WPU                  WPUCbits.WPUC3
#define UART_RX_OD                   ODCONCbits.ODCC3
#define UART_RX_ANS                  ANSELCbits.ANSC3
#define UART_RX_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define UART_RX_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define UART_RX_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define UART_RX_GetValue()           PORTCbits.RC3
#define UART_RX_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define UART_RX_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define UART_RX_SetPullup()          do { WPUCbits.WPUC3 = 1; } while(0)
#define UART_RX_ResetPullup()        do { WPUCbits.WPUC3 = 0; } while(0)
#define UART_RX_SetPushPull()        do { ODCONCbits.ODCC3 = 0; } while(0)
#define UART_RX_SetOpenDrain()       do { ODCONCbits.ODCC3 = 1; } while(0)
#define UART_RX_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define UART_RX_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)
// get/set IO_RC4 aliases
#define UART_TX_TRIS                 TRISCbits.TRISC4
#define UART_TX_LAT                  LATCbits.LATC4
#define UART_TX_PORT                 PORTCbits.RC4
#define UART_TX_WPU                  WPUCbits.WPUC4
#define UART_TX_OD                   ODCONCbits.ODCC4
#define UART_TX_ANS                  ANSELCbits.ANSC4
#define UART_TX_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define UART_TX_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define UART_TX_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define UART_TX_GetValue()           PORTCbits.RC4
#define UART_TX_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define UART_TX_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define UART_TX_SetPullup()          do { WPUCbits.WPUC4 = 1; } while(0)
#define UART_TX_ResetPullup()        do { WPUCbits.WPUC4 = 0; } while(0)
#define UART_TX_SetPushPull()        do { ODCONCbits.ODCC4 = 0; } while(0)
#define UART_TX_SetOpenDrain()       do { ODCONCbits.ODCC4 = 1; } while(0)
#define UART_TX_SetAnalogMode()      do { ANSELCbits.ANSC4 = 1; } while(0)
#define UART_TX_SetDigitalMode()     do { ANSELCbits.ANSC4 = 0; } while(0)
// get/set IO_RC5 aliases
#define INT_IN_TRIS                 TRISCbits.TRISC5
#define INT_IN_LAT                  LATCbits.LATC5
#define INT_IN_PORT                 PORTCbits.RC5
#define INT_IN_WPU                  WPUCbits.WPUC5
#define INT_IN_OD                   ODCONCbits.ODCC5
#define INT_IN_ANS                  ANSELCbits.ANSC5
#define INT_IN_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define INT_IN_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define INT_IN_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define INT_IN_GetValue()           PORTCbits.RC5
#define INT_IN_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define INT_IN_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define INT_IN_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define INT_IN_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define INT_IN_SetPushPull()        do { ODCONCbits.ODCC5 = 0; } while(0)
#define INT_IN_SetOpenDrain()       do { ODCONCbits.ODCC5 = 1; } while(0)
#define INT_IN_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define INT_IN_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)
#define RC5_SetInterruptHandler  INT_IN_SetInterruptHandler
// get/set IO_RC6 aliases
#define INT_OUT_TRIS                 TRISCbits.TRISC6
#define INT_OUT_LAT                  LATCbits.LATC6
#define INT_OUT_PORT                 PORTCbits.RC6
#define INT_OUT_WPU                  WPUCbits.WPUC6
#define INT_OUT_OD                   ODCONCbits.ODCC6
#define INT_OUT_ANS                  ANSELCbits.ANSC6
#define INT_OUT_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define INT_OUT_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define INT_OUT_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define INT_OUT_GetValue()           PORTCbits.RC6
#define INT_OUT_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define INT_OUT_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define INT_OUT_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define INT_OUT_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define INT_OUT_SetPushPull()        do { ODCONCbits.ODCC6 = 0; } while(0)
#define INT_OUT_SetOpenDrain()       do { ODCONCbits.ODCC6 = 1; } while(0)
#define INT_OUT_SetAnalogMode()      do { ANSELCbits.ANSC6 = 1; } while(0)
#define INT_OUT_SetDigitalMode()     do { ANSELCbits.ANSC6 = 0; } while(0)
// get/set IO_RC7 aliases
#define TP2_TRIS                 TRISCbits.TRISC7
#define TP2_LAT                  LATCbits.LATC7
#define TP2_PORT                 PORTCbits.RC7
#define TP2_WPU                  WPUCbits.WPUC7
#define TP2_OD                   ODCONCbits.ODCC7
#define TP2_ANS                  ANSELCbits.ANSC7
#define TP2_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define TP2_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define TP2_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define TP2_GetValue()           PORTCbits.RC7
#define TP2_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define TP2_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define TP2_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define TP2_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define TP2_SetPushPull()        do { ODCONCbits.ODCC7 = 0; } while(0)
#define TP2_SetOpenDrain()       do { ODCONCbits.ODCC7 = 1; } while(0)
#define TP2_SetAnalogMode()      do { ANSELCbits.ANSC7 = 1; } while(0)
#define TP2_SetDigitalMode()     do { ANSELCbits.ANSC7 = 0; } while(0)
/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize (void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handling routine
 * @param none
 * @return none
 */
void PIN_MANAGER_IOC(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handler for the SWITCH1 pin functionality
 * @param none
 * @return none
 */
void SWITCH1_ISR(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for SWITCH1 pin interrupt-on-change functionality.
 *        Allows selecting an interrupt handler for SWITCH1 at application runtime.
 * @pre Pins intializer called
 * @param InterruptHandler function pointer.
 * @return none
 */
void SWITCH1_SetInterruptHandler(void (* InterruptHandler)(void));

/**
 * @ingroup  pinsdriver
 * @brief Dynamic Interrupt Handler for SWITCH1 pin.
 *        This is a dynamic interrupt handler to be used together with the SWITCH1_SetInterruptHandler() method.
 *        This handler is called every time the SWITCH1 ISR is executed and allows any function to be registered at runtime.
 * @pre Pins intializer called
 * @param none
 * @return none
 */
extern void (*SWITCH1_InterruptHandler)(void);

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for SWITCH1 pin. 
 *        This is a predefined interrupt handler to be used together with the SWITCH1_SetInterruptHandler() method.
 *        This handler is called every time the SWITCH1 ISR is executed. 
 * @pre Pins intializer called
 * @param none
 * @return none
 */
void SWITCH1_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handler for the SWITCH2 pin functionality
 * @param none
 * @return none
 */
void SWITCH2_ISR(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for SWITCH2 pin interrupt-on-change functionality.
 *        Allows selecting an interrupt handler for SWITCH2 at application runtime.
 * @pre Pins intializer called
 * @param InterruptHandler function pointer.
 * @return none
 */
void SWITCH2_SetInterruptHandler(void (* InterruptHandler)(void));

/**
 * @ingroup  pinsdriver
 * @brief Dynamic Interrupt Handler for SWITCH2 pin.
 *        This is a dynamic interrupt handler to be used together with the SWITCH2_SetInterruptHandler() method.
 *        This handler is called every time the SWITCH2 ISR is executed and allows any function to be registered at runtime.
 * @pre Pins intializer called
 * @param none
 * @return none
 */
extern void (*SWITCH2_InterruptHandler)(void);

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for SWITCH2 pin. 
 *        This is a predefined interrupt handler to be used together with the SWITCH2_SetInterruptHandler() method.
 *        This handler is called every time the SWITCH2 ISR is executed. 
 * @pre Pins intializer called
 * @param none
 * @return none
 */
void SWITCH2_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt on Change Handler for the INT_IN pin functionality
 * @param none
 * @return none
 */
void INT_IN_ISR(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for INT_IN pin interrupt-on-change functionality.
 *        Allows selecting an interrupt handler for INT_IN at application runtime.
 * @pre Pins intializer called
 * @param InterruptHandler function pointer.
 * @return none
 */
void INT_IN_SetInterruptHandler(void (* InterruptHandler)(void));

/**
 * @ingroup  pinsdriver
 * @brief Dynamic Interrupt Handler for INT_IN pin.
 *        This is a dynamic interrupt handler to be used together with the INT_IN_SetInterruptHandler() method.
 *        This handler is called every time the INT_IN ISR is executed and allows any function to be registered at runtime.
 * @pre Pins intializer called
 * @param none
 * @return none
 */
extern void (*INT_IN_InterruptHandler)(void);

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for INT_IN pin. 
 *        This is a predefined interrupt handler to be used together with the INT_IN_SetInterruptHandler() method.
 *        This handler is called every time the INT_IN ISR is executed. 
 * @pre Pins intializer called
 * @param none
 * @return none
 */
void INT_IN_DefaultInterruptHandler(void);


#endif // PINS_H
/**
 End of File
*/