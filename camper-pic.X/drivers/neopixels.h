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

// Declare public functions for the NeoPixel control
void neopixel_send_color(uint8_t red, uint8_t green, uint8_t blue);
void neopixel_reset(void);
void neopixel_clear(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NEOPIXELS_H */

