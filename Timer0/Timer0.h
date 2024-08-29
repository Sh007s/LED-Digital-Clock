#ifndef TIMER0_H
#define TIMER0_H

#include <esp32-hal-timer.h>
#include "driver/timer.h"
#include "soc/soc_caps.h"

// define the LED pin to Toggle
#define LED_PIN 2

extern volatile bool timer0_Flag;   // Declare the flag with extern
extern volatile int32_t CountIsrAT; // Declare the counter with extern

void init_timer0();
void IRAM_ATTR onTimer();

#endif
