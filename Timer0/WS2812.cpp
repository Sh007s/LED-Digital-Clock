#include "WS2812.h"

NeoPixelBus<NeoGrbFeature, NeoEsp32I2s1800KbpsMethod> strip(LED_COUNT, LED_PIN1);

//// Declare the variables
// Define a pattern using a 64-bit bitmask, split into two 32-bit integers for Segment 1
uint32_t targetLED0[10][2] = {
  {0b10010000100100001001000001100000, 0b0000000011000001001000010010000},  // LED 0
  {0b10000000100000001000000010000000, 0b00000000100000001000000010000000}, // LED 1
  {0b01100000100000001000000001110000, 0b00000000111000000001000000010000}, // LED 2
  {0b01110000100000001000000001110000, 0b00000000011100001000000010000000}, // LED 3
  {0b01100000100100001001000010010000, 0b00000000100000001000000010000000}, // LED 4
  {0b01100000000100000001000011100000, 0b00000000011100001000000010000000}, // LED 5
  {0b01100000000100000001000011100000, 0b00000000011000001001000010010000}, // LED 6
  {0b10000000100000001000000001110000, 0b00000000100000001000000010000000}, // LED 7
  {0b01100000100100001001000001100000, 0b00000000011000001001000010010000}, // LED 8
  {0b11100000100100001001000001100000, 0b00000000100000001000000010000000}, // LED 9
};

// Define a pattern using a 64-bit bitmask, split into two 32-bit integers for Segment 2
uint32_t targetLED1[10][2] = {
  {0b00001001000010010000100100000110, 0b00000000000001100000100100001001}, // LED 0
  {0b00001000000010000000100000001000, 0b00000000000010000000100000001000}, // LED 1
  {0b00000110000010000000100000000111, 0b00000000000011100000000100000001}, // LED 2
  {0b00000111000010000000100000000111, 0b00000000000001110000100000001000}, // LED 3
  {0b00000110000010010000100100001001, 0b00000000000010000000100000001000}, // LED 4
  {0b00000110000000010000000100001110, 0b00000000000001110000100000001000}, // LED 5
  {0b00000110000000010000000100001110, 0b00000000000001100000100100001001}, // LED 6
  {0b00001000000010000000100000000111, 0b00000000000010000000100000001000}, // LED 7
  {0b00000110000010010000100100000110, 0b00000000000001100000100100001001}, // LED 8
  {0b00001110000010010000100100000110, 0b00000000000010000000100000001000}, // LED 9
};

u_int8_t brightness = 1;

void init_ledsec()
{
  strip.Begin();
  strip.Show();  // Initialize all pixels to 'off'
  displayDigit(0,0);
}

void updateLEDs(int ledIndex, RgbColor color) {
  if (ledIndex >= 0 && ledIndex < LED_COUNT) {  // Check bounds
    strip.SetPixelColor(ledIndex, color);
  }
}

void clearLEDs() {
  for (int i = 0; i < LED_COUNT; i++) {
    updateLEDs(i, RgbColor(0, 0, 0)); // Turn off all LEDs
  }
}

void displayDigit(int tens, int ones) 
{
  // Validate digit values
    if (tens < 0 || tens > 9 || ones < 0 || ones > 9)
     {
        Serial.println("Digit out of range");
        return;
     }
    
    clearLEDs(); // Turn off all LEDs
      
      // Display digit for Segment 1 (tens digit)
    for (int i = 0; i < 32; i++) 
    {
      if (targetLED1[tens][0] & (1 << i)) 
      {
        updateLEDs(i, RgbColor((BRIGHT *brightness/RED), (BRIGHT *brightness/GREEN), (BRIGHT *brightness/BLUE))); // Turn on LEDs for Segment 1 (low 32 bits)
      }
        
      if (targetLED1[tens][1] & (1 << i))
      {
        updateLEDs(i + 32, RgbColor((BRIGHT *brightness/RED), (BRIGHT *brightness/GREEN), (BRIGHT *brightness/BLUE))); // Turn on LEDs for Segment 1 (high 32 bits)
      }
    }
      
      // Display digit for Segment 0 (ones digit)
    for (int i = 0; i < 32; i++)
    {
     if (targetLED0[ones][0] & (1 << i)) 
      {
         updateLEDs(i, RgbColor((BRIGHT *brightness/RED), (BRIGHT *brightness/GREEN), (BRIGHT *brightness/BLUE)));// Turn on LEDs for Segment 0 (low 32 bits)
      }
     if (targetLED0[ones][1] & (1 << i))
      {
        updateLEDs(i + 32, RgbColor((BRIGHT *brightness/RED), (BRIGHT *brightness/GREEN), (BRIGHT *brightness/BLUE))); // Turn on LEDs for Segment 0 (high 32 bits)
      }
    }
   strip.Show(); // Display the updated LED states
}
