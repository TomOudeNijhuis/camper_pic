/* 
 * File:   neopixels.h
 * Author: oudenijhuis
 *
 * Created on September 30, 2024, 8:18 PM
 */

#ifndef NEOPIXELS_H
#define	NEOPIXELS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

// Timing definitions for WS2812 (in clock cycles)
// Adjust these for your clock speed as needed
#define T0H() asm("nop");        // ~0.35 탎 high time for '0' bit
#define T0L() __delay_us(1);     // ~0.80 탎 low time for '0' bit
#define T1H() __delay_us(1);     // ~0.70 탎 high time for '1' bit
#define T1L() asm("nop");        // ~0.60 탎 low time for '1' bit

// Declare public functions for the NeoPixel control
void neopixel_send_color(uint8_t red, uint8_t green, uint8_t blue);
void neopixel_reset(void);
void neopixel_clear(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NEOPIXELS_H */

