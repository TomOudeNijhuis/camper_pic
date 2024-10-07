/*
    // Example 1: Send color by name (Red)
    Color color;
    color.colorName = RED;
    neopixel_send_color(color);

    // Example 2: Send color by direct RGB value (Custom Color)
    color.colorValue.red = 100;
    color.colorValue.green = 150;
    color.colorValue.blue = 200;
    neopixel_send_color(color);
 */
#include <stdint.h>
#include "../neopixels.h"
#include "../../mcc_generated_files/system/system.h"

void neopixel_send_byte(uint8_t byte);

// Timing definitions for WS2812 (in clock cycles)
// One instruction cycle = 1 / (_XTAL_FREQ / 4) = 125 ns
#define SEND_BIT(b)             \
    if(b) {                     \
        NEOPIXEL_LAT = 1;            \
        NOP(); NOP(); NOP();     /* High period for '1' bit (~0.35µs) */   \
        NEOPIXEL_LAT = 0;            \
    } else {                    \
        NEOPIXEL_LAT = 1;            \
        NOP();                  /* High period for '0' bit (~0.35µs) */  \
        NEOPIXEL_LAT = 0;            \
        NOP(); NOP();           /* Low period for '0' bit (~0.9µs) */    \
    }

ColorName led1Color = RED;
uint8_t led1Dutycycle = 0;
ColorName led2Color = RED;
uint8_t led2Dutycycle = 0;
uint8_t counter = 0;

// Function to send a byte to the NeoPixel
void neopixel_send_byte(uint8_t databyte) {
    SEND_BIT(databyte & 0b10000000);
    SEND_BIT(databyte & 0b01000000);
    SEND_BIT(databyte & 0b00100000);
    SEND_BIT(databyte & 0b00010000);
    SEND_BIT(databyte & 0b00001000);
    SEND_BIT(databyte & 0b00000100);
    SEND_BIT(databyte & 0b00000010);
    SEND_BIT(databyte & 0b00000001);
}

// Function to send RGB color data to the NeoPixel
void neopixel_send_color(ColorName colorname) {
    if (colorname < COLOR_COUNT) {
        RGBColor rgb = colorTable[colorname];
        
        neopixel_send_byte(rgb.green);
        neopixel_send_byte(rgb.red);
        neopixel_send_byte(rgb.blue);
    }
}

// Function to reset the NeoPixel (latch the data)
void neopixel_reset(void) {
    // Hold the line low for at least 50 us
    NEOPIXEL_LAT = 0;
    __delay_us(60);  // Wait 60 us to reset the pixels
}

// Function to clear the NeoPixels (turn off all LEDs)
void neopixel_clear(void) {
    // Send zeros to turn off the LEDs
    neopixel_send_color(BLACK);
    neopixel_send_color(BLACK);  // Adjust for number of LEDs in series
    neopixel_reset();
}

void neopixel_setcolor(uint8_t led, ColorName colorname, uint8_t dutycycle) {
    if (led == 1) {
        led1Color = colorname;
        led1Dutycycle = dutycycle;
    } else if (led == 2) {
        led2Color = colorname;
        led2Dutycycle = dutycycle;
    }
}

void neopixel_runner(void) {
    if (counter == 0) {
        neopixel_send_color(led1Color);
        neopixel_send_color(led2Color);
    } else if ((counter == led1Dutycycle) && (counter == led2Dutycycle)) {
        neopixel_send_color(BLACK);
        neopixel_send_color(BLACK);
    } else if (counter == led1Dutycycle) {
        neopixel_send_color(BLACK);
        neopixel_send_color(led2Color);
    } else if (counter == led2Dutycycle) {
        neopixel_send_color(led1Color);
        neopixel_send_color(BLACK);
    }
    
    if (counter < LED_PERIOD) {
        counter++;
    } else
        counter = 0;
}