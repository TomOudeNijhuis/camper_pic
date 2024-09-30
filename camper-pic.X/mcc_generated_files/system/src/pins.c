/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.0.0
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

#include "../pins.h"

void (*SWITCH1_InterruptHandler)(void);
void (*SWITCH2_InterruptHandler)(void);
void (*INT_IN_InterruptHandler)(void);

void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x0;
    LATC = 0x0;
    LATE = 0x0;

    /**
    TRISx registers
    */
    TRISA = 0xFF;
    TRISB = 0xDC;
    TRISC = 0x2B;
    TRISE = 0x8;

    /**
    ANSELx registers
    */
    ANSELA = 0x0;
    ANSELB = 0x1C;
    ANSELC = 0x3;

    /**
    WPUx registers
    */
    WPUA = 0x0;
    WPUB = 0xC0;
    WPUC = 0x0;
    WPUE = 0x0;
  
    /**
    ODx registers
    */
   
    ODCONA = 0x0;
    ODCONB = 0x0;
    ODCONC = 0x0;
    /**
    SLRCONx registers
    */
    SLRCONA = 0xFF;
    SLRCONB = 0xFF;
    SLRCONC = 0xFF;
    /**
    INLVLx registers
    */
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;
    INLVLE = 0x8;

    /**
    PPS registers
    */
    RX1PPS = 0x13; //RC3->EUSART1:RX1;
    RC4PPS = 0x0E;  //RC4->EUSART1:TX1;

    /**
    APFCON registers
    */

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x0;
    IOCAF = 0x0;
    IOCBP = 0x0;
    IOCBN = 0xC0;
    IOCBF = 0x0;
    IOCCP = 0x0;
    IOCCN = 0x20;
    IOCCF = 0x0;
    IOCEP = 0x0;
    IOCEN = 0x0;
    IOCEF = 0x0;

    SWITCH1_SetInterruptHandler(SWITCH1_DefaultInterruptHandler);
    SWITCH2_SetInterruptHandler(SWITCH2_DefaultInterruptHandler);
    INT_IN_SetInterruptHandler(INT_IN_DefaultInterruptHandler);

    // Enable PIE0bits.IOCIE interrupt 
    PIE0bits.IOCIE = 1; 
}
  
void PIN_MANAGER_IOC(void)
{
    // interrupt on change for pin SWITCH1}
    if(IOCBFbits.IOCBF6 == 1)
    {
        SWITCH1_ISR();  
    }
    // interrupt on change for pin SWITCH2}
    if(IOCBFbits.IOCBF7 == 1)
    {
        SWITCH2_ISR();  
    }
    // interrupt on change for pin INT_IN}
    if(IOCCFbits.IOCCF5 == 1)
    {
        INT_IN_ISR();  
    }
}
   
/**
   SWITCH1 Interrupt Service Routine
*/
void SWITCH1_ISR(void) {

    // Add custom IOCBF6 code

    // Call the interrupt handler for the callback registered at runtime
    if(SWITCH1_InterruptHandler)
    {
        SWITCH1_InterruptHandler();
    }
    IOCBFbits.IOCBF6 = 0;
}

/**
  Allows selecting an interrupt handler for IOCBF6 at application runtime
*/
void SWITCH1_SetInterruptHandler(void (* InterruptHandler)(void)){
    SWITCH1_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCBF6
*/
void SWITCH1_DefaultInterruptHandler(void){
    // add your SWITCH1 interrupt custom code
    // or set custom function using SWITCH1_SetInterruptHandler()
}
   
/**
   SWITCH2 Interrupt Service Routine
*/
void SWITCH2_ISR(void) {

    // Add custom IOCBF7 code

    // Call the interrupt handler for the callback registered at runtime
    if(SWITCH2_InterruptHandler)
    {
        SWITCH2_InterruptHandler();
    }
    IOCBFbits.IOCBF7 = 0;
}

/**
  Allows selecting an interrupt handler for IOCBF7 at application runtime
*/
void SWITCH2_SetInterruptHandler(void (* InterruptHandler)(void)){
    SWITCH2_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCBF7
*/
void SWITCH2_DefaultInterruptHandler(void){
    // add your SWITCH2 interrupt custom code
    // or set custom function using SWITCH2_SetInterruptHandler()
}
   
/**
   INT_IN Interrupt Service Routine
*/
void INT_IN_ISR(void) {

    // Add custom IOCCF5 code

    // Call the interrupt handler for the callback registered at runtime
    if(INT_IN_InterruptHandler)
    {
        INT_IN_InterruptHandler();
    }
    IOCCFbits.IOCCF5 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF5 at application runtime
*/
void INT_IN_SetInterruptHandler(void (* InterruptHandler)(void)){
    INT_IN_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF5
*/
void INT_IN_DefaultInterruptHandler(void){
    // add your INT_IN interrupt custom code
    // or set custom function using INT_IN_SetInterruptHandler()
}
/**
 End of File
*/