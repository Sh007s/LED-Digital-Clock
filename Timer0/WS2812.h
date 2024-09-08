#ifndef WS2812_H
#define WS2812_H

#include <FastLED.h>

#define LED_PIN1         5
#define LED_COUNT        64
#define COLOR_ORDER      RGB
#define RED              10
#define GREEN            110
#define BLUE             10
#define BRIGHT           255

// Declare the variables with extern
extern uint8_t brightness;
extern CRGB leds[LED_COUNT];

// Function declarations
void init_ledsec();
void displayDigit(int tens, int one);
void updateLEDs(int ledIndex, CRGB color);
void clearLEDs();

#endif // WS2812_H