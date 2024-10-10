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

#define MAX_COMMAND_LENGTH 16
#define OVERRULE_TIME 100

static const struct TMR_INTERFACE *Timer = &Timer0;

// Function to handle received commands
void handle_command(char *command);

/*
    Main application
*/
uint8_t neopixel_overruled_counter = 0;

static void Timer_Callback(void)
{
    TP2_LAT = ~TP2_LAT;
    powerRunner();
    neopixel_runner();
    
    if (neopixel_overruled_counter > 0) {
        neopixel_overruled_counter --;
    }
}

int main(void)
{
    uint8_t state1;
    uint8_t state2;
    uint8_t switch1_prev;
    uint8_t switch2_prev;
    
    SYSTEM_Initialize();
    powerInitialize();

    SWITCH1_SetDigitalInput();
    SWITCH2_SetDigitalInput();
    NEOPIXEL_SetDigitalOutput();
    INT_OUT_SetDigitalOutput();
    TP2_SetDigitalOutput();
    
    EUSART1_Enable();
    Timer->TimeoutCallbackRegister(Timer_Callback);
    
    INTERRUPT_GlobalInterruptEnable(); 
 	INTERRUPT_PeripheralInterruptEnable(); 
   
    neopixel_overruled_counter = OVERRULE_TIME;
    neopixel_setcolor(1, TEAL, 0);
    neopixel_setcolor(2, TEAL, 0);
    
    INT_OUT_LAT = 1;
    printf("Camper Interface Running\r\n");
    __delay_ms(25);

    INT_OUT_LAT = 0;
    TP2_LAT = 0;

    __delay_ms(25);
    
    while(1)
    {
        if (EUSART1_IsRxReady()) {
            char received_char = EUSART1_Read();  // Read a character from EUSART buffer

            if (received_char == '\r') {
                putch('\r');
                putch('\n');
                rx_buffer[buffer_index] = '\0';  // Null-terminate the string
                handle_command(rx_buffer);       // Process the received command
                buffer_index = 0;                // Reset buffer index
            } else if (received_char == '\b') {
                if (buffer_index > 0) {
                    buffer_index--;
                    rx_buffer[buffer_index] = '\0';
                            
                    putch('\b');
                    putch(' ');
                    putch('\b');
                }
            } else {
                // Store the character in the buffer
                if (buffer_index < (BUFFER_SIZE - 1)) {
                    rx_buffer[buffer_index++] = received_char;
                    putch(received_char);
                }
            }
        }
        // read button values
        if (SWITCH1_PORT == 0 && switch1_prev == 1) {
            switch1_prev = 0;
            
            state2 = getPumpState();
            
            if (state2 == 0) {
                setPump(1);
            } else {
                setPump(0);
            }
        } else if (SWITCH1_PORT == 1) {
            switch1_prev = 1;
        }
        
        if (SWITCH2_PORT == 0 && switch2_prev == 1) {
            switch2_prev = 0;
            
            state1 = getHouseholdState();
            
            if (state1 == OFF) {
                setHousehold(1);
            } else {
                setHousehold(0);
            }
        } else if (SWITCH2_PORT == 1) {
            switch2_prev = 1;
        }
        
        // Update neopixels
        if (neopixel_overruled_counter == 0) {
            state1 = (uint8_t)getHouseholdState();

            if (state1 == OFF) {
                neopixel_setcolor(1, BLACK, 0);
            } else if (state1 == PENDING) {
                neopixel_setcolor(1, YELLOW, 0);
            } else {
                neopixel_setcolor(1, GREEN, 0);
            }

            state2 = (uint8_t)getPumpState();

            if (state2 == 0) {
                neopixel_setcolor(2, BLACK, 0);
            } else {
                neopixel_setcolor(2, GREEN, 0);
            }        
        }
    }    
}

// Function to handle received commands
void handle_command(char *command_str) {
    char command[MAX_COMMAND_LENGTH] = {0};
    char param_str[MAX_COMMAND_LENGTH] = {0};
    char param2_str[MAX_COMMAND_LENGTH] = {0};
    
    uint8_t param_value = 0;
    uint8_t i;
    uint16_t temp;
    
    sscanf(command_str, "%s %s %s", command, param_str, param2_str);
    
    // Handle the command
    if (strcmp(command, "NEOPIXEL1") == 0) {
        if (strlen(param2_str) > 0)
            param_value = (uint8_t)atoi(param2_str);
        else
            param_value = 0;
        
        if (strcmp(param_str, "cyan") == 0) {
            neopixel_setcolor(1, CYAN, param_value);
            neopixel_overruled_counter = OVERRULE_TIME;
            printf("NEOPIXEL1 state=%s\r\n", "cyan");
        } else if (strcmp(param_str, "olive") == 0) {
            neopixel_setcolor(1, OLIVE, param_value);
            neopixel_overruled_counter = OVERRULE_TIME;
            printf("NEOPIXEL1 state=%s\r\n", "olive");
        } else if (strcmp(param_str, "silver") == 0) {
            neopixel_setcolor(1, SILVER, param_value);
            neopixel_overruled_counter = OVERRULE_TIME;
            printf("NEOPIXEL1 state=%s\r\n", "silver");
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Options are NEOPIXEL1 <color: CYAN, OLIVE, SILVER> <dutycycle: <0-10>\r\n");
        }
    } else if (strcmp(command, "NEOPIXEL2") == 0) {
        if (strlen(param2_str) > 0)
            param_value = (uint8_t)atoi(param2_str);
        else
            param_value = 0;

        if (strcmp(param_str, "cyan") == 0) {
            neopixel_setcolor(2, CYAN, param_value);
            neopixel_overruled_counter = OVERRULE_TIME;
            printf("NEOPIXEL2 state=%s\r\n", "cyan");
        } else if (strcmp(param_str, "olive") == 0) {
            neopixel_setcolor(2, OLIVE, param_value);
            printf("NEOPIXEL2 state=%s\r\n", "olive");
            neopixel_overruled_counter = OVERRULE_TIME;
        } else if (strcmp(param_str, "silver") == 0) {
            neopixel_setcolor(2, SILVER, param_value);
            neopixel_overruled_counter = OVERRULE_TIME;
            printf("NEOPIXEL2 state=%s\r\n", "silver");
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use NEOPIXEL2 <color: CYAN, OLIVE, SILVER> <dutycycle: <0-10>\r\n");
        }
    } else if (strcmp(command, "HOUSEHOLD") == 0) {
        if (strcmp(param_str, "?") == 0) {
            const char* status = getHouseHoldStateStr();
            printf("HOUSEHOLD state=%s\r\n", status);
        } else if ((strcmp(param_str, "0") == 0) || (strcmp(param_str, "1") == 0)) {
            param_value = (uint8_t)atoi(param_str);
            setHousehold(param_value);

            const char* status = getHouseHoldStateStr();
            printf("HOUSEHOLD state=%s\r\n", status);            
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use HOUSEHOLD <state: ?, 0, 1>\r\n");
        }
    } else if (strcmp(command, "PUMP") == 0) {
        if (strcmp(param_str, "?") == 0) {
            i = getPumpState();
            printf("PUMP state=%d\r\n", i);
        } else if ((strcmp(param_str, "0") == 0) || (strcmp(param_str, "1") == 0)) {
            param_value = (uint8_t)atoi(param_str);
            setPump(param_value);

            i = getPumpState();
            printf("PUMP state=%d\r\n", i);      
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use PUMP <state: ?, 0, 1>\r\n");
        }
    } else if (strcmp(command, "VOLTAGE") == 0) {
        if (strcmp(param_str, "household") == 0) {
            temp = getVoltage(V_HOUSEHOLD);
            printf("VOLTAGE household=%d\r\n", temp);      
        } else if (strcmp(param_str, "mains") == 0) {
            temp = getVoltage(V_MAINS);
            printf("VOLTAGE mains=%d\r\n", temp);      
        } else if (strcmp(param_str, "starter") == 0) {
            temp = getVoltage(V_STARTER);
            printf("VOLTAGE starter=%d\r\n", temp);      
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use VOLTAGE <channel: household, mains, starter>\r\n");
        }
    } else if (strcmp(command, "WATER") == 0) {
        if (strcmp(param_str, "?") == 0) {
            i = getWater();
            printf("WATER value=%d\r\n", i);
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use WATER <state: ?>\r\n");
        }
    } else if (strcmp(command, "WASTE") == 0) {
        if (strcmp(param_str, "?") == 0) {
            i = getWaste();
            printf("WASTE value=%d\r\n", i);
        } else {
            printf("Unknown parameter: %s\r\n", param_str);
            printf("Use WASTE <state: ?>\r\n");
        }
    } else {
        // Handle unknown command
        printf("Unknown command: %s\r\n", command);
        printf("Implemented commands are NEOPIXEL1, NEOPIXEL2, HOUSEHOLD, PUMP, VOLTAGE, WATER, WASTE.\r\n");
    }
}