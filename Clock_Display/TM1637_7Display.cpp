#include "TM1637_7Display.h"
#include <Arduino.h>

TM1637Display display(CLK, DIO);

void init_7dispaly(void) {
  display.setBrightness(0x0f);

  // Clear display at start
  display.clear();
}

void showDispaly(float displayTemp_Hum)
{
  int sample = displayTemp_Hum * 100;
  display.showNumberDecEx(sample, 0x40, true, 4, 0);
  delay(500);
}