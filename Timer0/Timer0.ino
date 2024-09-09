#include "Timer0.h"
#include "WS2812.h"
#include "GIPO_PIN.h"

volatile uint32_t second = 0;
int mins = 0;
int hours = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  Serial.begin(115200);      // Initialize Serial Monitor
  init_timer0();             // Initialized the timer0
  init_ledsec();             // Initialized the LED strips
}

void loop() {
  // Check if timer0 has enabled
  if (timer0_Flag) {
    // Clear the flag
    timer0_Flag = false;

    // Reset CountIsrAT after 60 seconds
    if (CountIsrAT >= 60) {
      CountIsrAT = 0;
      mins++;
      if (mins >= 60) {
        mins = 0;
        hours++;
        if (hours >= 12) {
          hours = 0;
        }
      }
      // Display `mins` on led2 strip when incremented
      //  displayDigit(mins / 10, mins % 10);  // Update the LED2 strip with `mins` value
    }

    // Print the current time
    Serial.printf("%02d hours %02d mins %02d sec\n", hours, mins, CountIsrAT);

    // Display `CountIsrAT` on led1 strip (seconds)
    // Display seconds and minutes on separate LED strips
    displayDigit(hours / 10, hours % 10, led3);            // Display minutes on LED strip 2
    displayDigit(mins / 10, mins % 10, led2);              // Display minutes on LED strip 2
    displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);  // Display seconds on LED strip 1
  }
}
