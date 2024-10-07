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
void neopixel_send_color(uint8_t red, uint8_t green, uint8_t blue) {
    // WS2812 expects data in GRB order
    neopixel_send_byte(green);
    neopixel_send_byte(red);
    neopixel_send_byte(blue);
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
    neopixel_send_color(0, 0, 0);
    neopixel_send_color(0, 0, 0);  // Adjust for number of LEDs in series
    neopixel_reset();
}
