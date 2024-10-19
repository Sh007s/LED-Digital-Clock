#include "Timer0.h"
#include "RTC.h"

hw_timer_t *timer = NULL;
volatile bool timer0_Flag = false;   // Define and initialize the flag
volatile int32_t CountIsrAT = -1;    // Define and initialize the counter to -1

// ISR for Timer 0
void IRAM_ATTR onTimer() {
    // Set the flag to indicate the timer interrupt occurred
    timer0_Flag = true;

    // Increment the counter
    CountIsrAT++;
   
    // Toggle the LED pin (optional: move to main logic if needed)
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

// Custom timer ISR function to handle time synchronization and other tasks
void timerISR() {
    static int second_counter = 0;  // Static variable to keep track of seconds
    second_counter++;

    // Sync time with RTC every 60 seconds
    if (second_counter >= 60) {
        sync_time_with_rtc();  // Function to sync time with RTC (assuming you have it defined)
        second_counter = 0;
    }

    // Other tasks based on the timer can be added here
}

// Function to initialize the hardware timer
void init_timer0() {
    // Initialize timer 0 with prescaler of 80 (for 1 MHz clock) and auto-reload enabled
    timer = timerBegin(0, 80, true);

    // Attach the ISR (onTimer) to timer 0
    timerAttachInterrupt(timer, &onTimer, true);

    // Set the timer to trigger every 1 second (1,000,000 microseconds)
    timerAlarmWrite(timer, 1000000, true);

    // Enable the timer alarm
    timerAlarmEnable(timer);
}
