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

// Define a table of 16 colors (RGB values)
const RGBColor colorTable[COLOR_COUNT] = {
    {255, 0, 0},    // Red
    {0, 255, 0},    // Green
    {0, 0, 255},    // Blue
    {255, 255, 0},  // Yellow
    {255, 0, 255},  // Magenta
    {0, 255, 255},  // Cyan
    {255, 255, 255},// White
    {128, 0, 0},    // Maroon
    {0, 128, 0},    // Dark Green
    {0, 0, 128},    // Navy
    {128, 128, 0},  // Olive
    {128, 0, 128},  // Purple
    {0, 128, 128},  // Teal
    {192, 192, 192},// Silver
    {128, 128, 128},// Gray
    {0, 0, 0}       // Black
};

// Declare public functions for the NeoPixel control
void neopixel_setcolor(uint8_t led, ColorName colorname, uint8_t dutycycle);

void neopixel_runner(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NEOPIXELS_H */

