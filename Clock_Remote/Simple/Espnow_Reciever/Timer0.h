#ifndef TIMER0_H
#define TIMER0_H

#include <esp32-hal-timer.h>
#include "driver/timer.h"
#include "soc/soc_caps.h"
//#include "GIPO_PIN.h" 
#define LED_PIN 2

extern hw_timer_t *timer;
extern volatile bool timer0_Flag;   // Declare the flag with extern
extern volatile int32_t CountIsrAT; // Declare the counter with extern

void init_timer0();
void IRAM_ATTR onTimer();

#endif
