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
#include <stdbool.h>

#define LED_PERIOD 10

// Define a structure for RGB colors
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGBColor;

typedef enum {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
    WHITE,
    MAROON,
    DARK_GREEN,
    NAVY,
    OLIVE,
    PURPLE,
    TEAL,
    SILVER,
    GRAY,
    BLACK,
    COLOR_COUNT  // This represents the number of colors
} ColorName;

/* Declare public functions for the NeoPixel control */
void neopixel_setcolor(uint8_t led, ColorName colorname, uint8_t dutycycle);

void neopixel_runner(void);

/* Suspend the state-driven LED updates in main.c for `ticks` TMR0 ticks
 * (~100 ms each) so a manually-set color stays visible. Called by the
 * SET_NEOPIXEL command handler and on startup. The counter is owned
 * here so commands.c can drive it without reaching into main.c. */
void neopixel_overrule(uint16_t ticks);
void neopixel_overrule_tick_isr(void);
bool neopixel_is_overruled(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NEOPIXELS_H */

