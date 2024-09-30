#include "../neopixels.h"
#include "../../mcc_generated_files/system/system.h"

void neopixel_send_bit(uint8_t bit);
void neopixel_send_byte(uint8_t byte);

// Function to send a single bit to the NeoPixel
void neopixel_send_bit(uint8_t bit) {
    if (bit) {
        // Send '1' bit
        NEOPIXEL_LAT = 1;
        T1H();
        NEOPIXEL_LAT = 0;
        T1L();
    } else {
        // Send '0' bit
        NEOPIXEL_LAT = 1;
        T0H();
        NEOPIXEL_LAT = 0;
        T0L();
    }
}

// Function to send a byte to the NeoPixel
void neopixel_send_byte(uint8_t byte) {
    for (int8_t i = 7; i >= 0; i--) {
        neopixel_send_bit((byte >> i) & 0x01);
    }
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
