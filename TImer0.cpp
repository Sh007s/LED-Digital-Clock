#include "Timer0.h"

hw_timer_t *timer = NULL;
volatile bool timer0_Flag = false;   // Define and initialize the flag
volatile int32_t CountIsrAT = -1;     //Define and initialize the counter is -1 

void IRAM_ATTR onTimer()
{
  //Set the flag to indicate the timer has occur
  timer0_Flag = true;
 
   CountIsrAT++;
  digitalWrite(LED_PIN,!digitalRead(LED_PIN));
}

void init_timer0()
{
   // Initialize timer with timer number 0, prescaler 80 (for 1MHz), and auto-reload enabled
  timer = timerBegin(0,80,true);

  //Attach onTimer function to our timer
  timerAttachInterrupt(timer, &onTimer, true);

  //Set alarm to call onTimer function every second
  timerAlarmWrite(timer,1000000,true);
  timerAlarmEnable(timer);
} 