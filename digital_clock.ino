#include "Timer0.h"
#include "WS2812.h"
#include "GIPO_PIN.h"
#include "Button.h"
#include "DHT11_Sensor.h"
#include "TM1637_7Display.h"
#include "Dot_Matrix.h"

volatile uint32_t second = 0;
bool timerPaused = false;  // Flag to track if the timer is paused
bool timersec = false;     // Flag to track if the sec is running
bool timermins = false;    // Flag to track if the mins is running
bool timerhours = false;   // Flag to track if the hours is running
bool button3Pressed = false;
int pressCount = 0;       // Counter to track number of `+` presses
int currentDayIndex = 0;  // Start with Week[0] (Sunday)
bool displayDay = false;  // Flag to control when to display the day

unsigned long lastButton3PressTime = 0;
int mins = 0, hours = 0, count = 0;
unsigned long dispalytemphum = 0;
int hourcount = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  Serial.begin(115200);      // Initialize Serial Monitor
  init_timer0();             // Initialized the timer0
  init_ledsec();             // Initialized the LED strips
  init_Button();             // Initialized the Button
  init_DHT11();              // Initialized the DHT11
  init_7dispaly();           // Initialized the TM1637 Dispaly
  init_DOT_setup();          // Initialize the MAX7219 module
}

void loop() {

  button1.loop();
  button2.loop();
  button3.loop();

  // Check if the button was pressed
  if (button1.isPressed()) {
    timerPaused = !timerPaused;  // Toggle the paused state

    if (timerPaused) {
      timerStop(timer);  // Pause the timer
      Serial.println("Timer Paused");
    } else {
      timerStart(timer);  // Resume the timer
      Serial.println("Timer Resumed");
    }
  }

  if (timerPaused && button2.isPressed()) {
    count = millis();
    Serial.println("Button2 is pressed");
  }

  if (timerPaused && button2.isReleased()) {
    unsigned long pressDuration = millis() - count;

    if (pressDuration >= LONG_PRESS_HOURS) {
      timerhours = true;
      timermins = timersec = false;
      Serial.print("Long press detected. Timer set to hours mode. timerhours: ");

    } else if (pressDuration >= MEDIUM_PRESS_MINS) {
      timermins = true;
      timersec = timerhours = false;
      Serial.print("Medium press detected. Timer set to minutes mode. timermins: ");

    } else if (pressDuration >= SHORT_PRESS_SEC) {
      timersec = true;
      timerhours = timermins = false;
      Serial.print("Short press detected. Timer set to seconds mode. timersec: ");

    } else {
      Serial.println("Button press too short. No action taken.");
    }
  }

  // Only check for button3 is press if the timer is paused
  if (timerPaused) {
    if (button3.isPressed() && !button3Pressed) {
      button3Pressed = true;
      lastButton3PressTime = millis();

      Serial.println("Button3 is pressed");
      if (timersec) {
        CountIsrAT = (CountIsrAT + 1) % 60;
        Serial.printf("Seconds: %d\n", CountIsrAT);
        displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);
      } else if (timermins) {
        mins = (mins + 1) % 60;
        Serial.printf("Minutes: %d\n", mins);
        displayDigit(mins / 10, mins % 10, led2);
      } else if (timerhours) {
        hours = (hours % 12) + 1;  // Cycle from 1 to 12
        Serial.printf("Hours: %d\n", hours);
        displayDigit(hours / 10, hours % 10, led3);
      } else {
        Serial.println("No timer mode active");
      }
    } else if (button3.isReleased() && button3Pressed) {
      if (millis() - lastButton3PressTime > 50) {  // Debounce check
        button3Pressed = false;
      }
    }
  }

  // Check if timer0 has enabled
  if (timer0_Flag) {
    timer0_Flag = false;  // Clear flag

    // Handle second rollover
    if (CountIsrAT >= 60) {
      CountIsrAT = 0;    // Reset seconds
      mins++;            // Increment minutes
      if (mins >= 60) {  // If minutes reach 60, reset and increment hours
        mins = 0;
        hours++;            // Increment hours
        if (hours >= 12) {  // Reset hours after 11:59:59 to 00:00:00
          hours = 0;
        }
      }
    }
    // Print the current time
    Serial.printf("%02d hours %02d mins %02d sec\n", hours, mins, CountIsrAT);

    displayDigit(hours / 10, hours % 10, led3);            // Display minutes on LED strip 3
    displayDigit(mins / 10, mins % 10, led2);              // Display minutes on LED strip 2
    displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);  // Display seconds on LED strip 1
  }

  float temperature = gettemp();
  float humidity = gethum();

  Serial.printf("Humidity: %.2f%%  Temperature: %.2f°C\n", humidity, temperature);

  showDispaly(temperature);
  showDispaly(humidity);
///
  hourcount = hours;
  if (hours > 11) {
    hourcount = hourcount + 12;
  }
  // Set flag to display the current day
  displayDay = true;

  // If we've reached 24 presses (24 hours), move to the next day
  if (hourcount >= 24) {
    pressCount = 0;     // Reset the counter for the new day
    currentDayIndex++;  // Move to the next day in the Week array

    // If we've reached the end of the week, loop back to Sunday (Week[0])
    if (currentDayIndex >= 7) {
      currentDayIndex = 0;
    }
  }

  // Debugging: Show the number of presses and the current day index
  Serial.print("hourcount: ");
  Serial.println(hourcount);
  Serial.print("Current day index: ");
  Serial.println(currentDayIndex);

  // Display the current day if flag is set
  if (displayDay || hourcount < 24) {
    scrollLeft(Week[currentDayIndex], counT[currentDayIndex]);  // Display current day
    delay(1000);                                                // Simulate hourly display
    displayDay = false;                                         // Reset flag
  }
}
/*
use to Rtos to run the task at same time 
*/
/*
Add the PM and AM 
while the increment the sec , mins and hours per led to on to indicate which is increment
add dot matrix for display for weeks 
*/