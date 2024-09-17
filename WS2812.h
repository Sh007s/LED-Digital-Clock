#ifndef WS2812_H
#define WS2812_H

#include <FastLED.h>
#include "GIPO_PIN.h"

#define LED_COUNT        64
#define COLOR_ORDER      RGB
#define RED              0
#define GREEN            200
#define BLUE             0
#define BRIGHT           50
#define LED_COUNT        64

// Declare the variables with extern
extern uint8_t brightness;
extern CRGB led1[LED_COUNT];  // LED strip for seconds
extern CRGB led2[LED_COUNT];  // LED strip for minutes
extern CRGB led3[LED_COUNT];  // LED strip for hours

// Function declarations
void init_ledsec();
void displayDigit(int tens, int ones, CRGB* leds);
void updateLEDs(CRGB* leds, int ledIndex, CRGB color);
void clearLEDs(CRGB* leds);

#endif // WS2812_H
