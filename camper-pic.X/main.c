 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
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
#include <string.h>
#include <stdio.h>
#include "mcc_generated_files/system/system.h"
#include "drivers/neopixels.h"
#include "drivers/power.h"

#define BUFFER_SIZE 64
char rx_buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;

// Function to handle received commands
void handle_command(char *command);

/*
    Main application
*/

int main(void)
{
    SYSTEM_Initialize();
    SWITCH1_SetDigitalInput();
    SWITCH2_SetDigitalInput();
    NEOPIXEL_SetDigitalOutput();
    INT_OUT_SetDigitalOutput();
    TP2_SetDigitalOutput();
    
    EUSART1_Enable();
            
    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
 	INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    // INT_OUT_SetPushPull();
    
    INT_OUT_LAT = 1;
    TP2_LAT = 1;
    printf("Camper Interface Running\r\n");
    __delay_ms(25);

    INT_OUT_LAT = 0;
    TP2_LAT = 0;

    neopixel_send_color(0xFF, 0, 0);  // Red color
    neopixel_send_color(0, 0xFF, 0);  // Green color
    neopixel_reset();

    __delay_ms(25);
    
    while(1)
    {
        if (EUSART1_IsRxReady()) {
            char received_char = EUSART1_Read();  // Read a character from EUSART buffer
            putch(received_char);      // Echo character
            
            // If we receive '\n' or '\r', treat it as the end of a command
            if (received_char == '\n' || received_char == '\r') {
                rx_buffer[buffer_index] = '\0';  // Null-terminate the string
                handle_command(rx_buffer);       // Process the received command
                buffer_index = 0;                // Reset buffer index
            } else {
                // Store the character in the buffer
                if (buffer_index < (BUFFER_SIZE - 1)) {
                    rx_buffer[buffer_index++] = received_char;
                }
            }
        }
                
    }    
}

// Function to handle received commands
void handle_command(char *command) {
    if (strcmp(command, "LEDS GREEN") == 0) {
        neopixel_send_color(0, 255, 0);
        neopixel_send_color(0, 255, 0);
        neopixel_reset();
        printf("LEDs to GREEN\r\n");
    } else if (strcmp(command, "POWER") == 0) {
        convert_powers();
    } else {
        printf("Unknown command: %s\r\n", command);
    }
}