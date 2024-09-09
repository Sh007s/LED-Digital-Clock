#include <FastLED.h>
#include "GIPO_PIN.h"
#include "WS2812.h"

CRGB led1[LED_COUNT];  // For seconds (on LED strip 1)
CRGB led2[LED_COUNT];  // For minutes (on LED strip 2)
CRGB led3[LED_COUNT];  // For minutes (on LED strip 3)

uint32_t targetLED1[10][2] = {
  { 0b10010000100100001001000001100000, 0b0000000011000001001000010010000 },   // 0
  { 0b10000000100000001000000010000000, 0b00000000100000001000000010000000 },  // 1
  { 0b01100000100000001000000001110000, 0b00000000111000000001000000010000 },  // 2
  { 0b01110000100000001000000001110000, 0b00000000011100001000000010000000 },  // 3
  { 0b01100000100100001001000010010000, 0b00000000100000001000000010000000 },  // 4
  { 0b01100000000100000001000011100000, 0b00000000011100001000000010000000 },  // 5
  { 0b01100000000100000001000011100000, 0b00000000011000001001000010010000 },  // 6
  { 0b10000000100000001000000001110000, 0b00000000100000001000000010000000 },  // 7
  { 0b01100000100100001001000001100000, 0b00000000011000001001000010010000 },  // 8
  { 0b11100000100100001001000001100000, 0b00000000100000001000000010000000 },  // 9
};

uint32_t targetLED0[10][2] = {
  { 0b00001001000010010000100100000110, 0b00000000000001100000100100001001 },  // 0
  { 0b00001000000010000000100000001000, 0b00000000000010000000100000001000 },  // 1
  { 0b00000110000010000000100000000111, 0b00000000000011100000000100000001 },  // 2
  { 0b00000111000010000000100000000111, 0b00000000000001110000100000001000 },  // 3
  { 0b00000110000010010000100100001001, 0b00000000000010000000100000001000 },  // 4
  { 0b00000110000000010000000100001110, 0b00000000000001110000100000001000 },  // 5
  { 0b00000110000000010000000100001110, 0b00000000000001100000100100001001 },  // 6
  { 0b00001000000010000000100000000111, 0b00000000000010000000100000001000 },  // 7
  { 0b00000110000010010000100100000110, 0b00000000000001100000100100001001 },  // 8
  { 0b00001110000010010000100100000110, 0b00000000000010000000100000001000 },  // 9
};

uint8_t brightness = 1;

void init_ledsec() {
  FastLED.addLeds<WS2812B, LED_PIN1, GRB>(led1, LED_COUNT);  // LED strip 1 for seconds
  FastLED.addLeds<WS2812B, LED_PIN2, GRB>(led2, LED_COUNT);  // LED strip 2 for minutes
  FastLED.addLeds<WS2812B, LED_PIN3, GRB>(led3, LED_COUNT);  // LED strip 3 for minutes
  FastLED.setBrightness(BRIGHT);
  FastLED.show();  // Initialize all pixels to 'off'
}

void updateLEDs(CRGB* leds, int ledIndex, CRGB color) {
  if (ledIndex >= 0 && ledIndex < LED_COUNT) {  // Check bounds
    leds[ledIndex] = color;
  }
}

void clearLEDs(CRGB* leds) {
  fill_solid(leds, LED_COUNT, CRGB::Black);
}

void displayDigit(int tens, int ones, CRGB* leds) {
  // Validate digit values
  if (tens < 0 || tens > 9 || ones < 0 || ones > 9) {
    Serial.println("Digit out of range");
    return;
  }

  clearLEDs(leds);  // Turn off all LEDs for the selected strip

  // Display tens digit
  for (int i = 0; i < 32; i++) {
    if (targetLED0[tens][0] & (1 << i)) {
      updateLEDs(leds, i, CRGB(BRIGHT * brightness / 3, BRIGHT * brightness / 3, BRIGHT * brightness / 3));
    }
    if (targetLED0[tens][1] & (1 << i)) {
      updateLEDs(leds, i + 32, CRGB(BRIGHT * brightness / 3, BRIGHT * brightness / 3, BRIGHT * brightness / 3));
    }
  }

  // Display ones digit
  for (int i = 0; i < 32; i++) {
    if (targetLED1[ones][0] & (1 << i)) {
      updateLEDs(leds, i, CRGB(BRIGHT * brightness / 3, BRIGHT * brightness / 3, BRIGHT * brightness / 3));
    }
    if (targetLED1[ones][1] & (1 << i)) {
      updateLEDs(leds, i + 32, CRGB(BRIGHT * brightness / 3, BRIGHT * brightness / 3, BRIGHT * brightness / 3));
    }
  }
   FastLED.show();  // Update the LEDs
}
