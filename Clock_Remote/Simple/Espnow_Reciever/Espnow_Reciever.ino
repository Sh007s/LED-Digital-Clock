#include "espnow.h"
#include "Timer0.h"

#define SHORT_PRESS_SEC 1000    // Duration for short press in milliseconds
#define MEDIUM_PRESS_MINS 2000  // Duration for medium press in milliseconds
#define LONG_PRESS_HOURS 3000   // Duration for long press in milliseconds

// Global variables
int mins = 0;
int hours = 0;
bool timerRunning = false;               // Flag to track if the timer is running
unsigned long lastButton3PressTime = 0;  // Stores the last press time of Button 3

void setup() {
  Serial.begin(115200);

  init_espnow();
  pinMode(LED_PIN, OUTPUT);
  init_timer0();  // Initialize timer

  // Initialize the received data structure
  Recvdata = { false, false, false, false, false, false, 0 };
}

void loop() {
  // Process received message (non-blocking)
  if (messageReceived) {
    // Print the received timer data for debugging
    Serial.println(Recvdata.timerPaused);
    Serial.print("Seconds running: ");
    Serial.println(Recvdata.timersec);
    Serial.print("Minutes running: ");
    Serial.println(Recvdata.timermins);
    Serial.print("Hours running: ");
    Serial.println(Recvdata.timerhours);

    // Reset the flag after processing
    messageReceived = false;
  }

  // Handle timer pausing and resuming based on received data
  if (Recvdata.timerPaused) {
    if (timerRunning) {
      timerStop(timer);  // Stop the timer if it's running
      Serial.println("Timer Paused");
      timerRunning = false;
    }
  } else {
    if (!timerRunning) {
      timerStart(timer);  // Start the timer if it's not running
      Serial.println("Timer Resumed");
      timerRunning = true;
    }
  }

  // Check for button 2 pressed status and press duration
  if (Recvdata.timerPaused && Recvdata.button2Pressed) {
    Serial.println(Recvdata.button2Pressed);
    Serial.println(Recvdata.pressDuration);

    // Set timer mode based on press duration
    if (Recvdata.pressDuration >= LONG_PRESS_HOURS) {
      Recvdata.timerhours = true;
      Recvdata.timermins = false;  // Ensure others are set to false
      Recvdata.timersec = false;
      Serial.println("Long press detected. Timer set to hours mode.");
    } else if (Recvdata.pressDuration >= MEDIUM_PRESS_MINS) {
      Recvdata.timermins = true;
      Recvdata.timerhours = false;  // Ensure others are set to false
      Recvdata.timersec = false;
      Serial.println("Medium press detected. Timer set to minutes mode.");
    } else if (Recvdata.pressDuration >= SHORT_PRESS_SEC) {
      Recvdata.timersec = true;
      Recvdata.timerhours = false;  // Ensure others are set to false
      Recvdata.timermins = false;
      Serial.println("Short press detected. Timer set to seconds mode.");
    } else {
      Serial.println("Button 2 press too short. No action taken.");
    }

    Recvdata.button2Pressed = false;  // Reset the button press flag after processing
  }

  // Check for button 3 pressed (increment the corresponding timer value)
  if (Recvdata.timerPaused && Recvdata.button3Pressed) {
    Serial.println("Button 3 press received");

    // Check which timer mode is active (seconds, minutes, or hours)
    if (Recvdata.timersec) {
      CountIsrAT = (CountIsrAT + 1) % 60;  // Increment seconds
      Serial.printf("Seconds running: %d\n", CountIsrAT);
    } else if (Recvdata.timermins) {
      mins = (mins + 1) % 60;  // Increment minutes
      Serial.printf("Minutes running: %d\n", mins);
    } else if (Recvdata.timerhours) {
      hours = (hours % 12) + 1;  // Increment hours, cycling from 1 to 12
      Serial.printf("Hours running: %d\n", hours);
    } else {
      Serial.println("No timer mode active");
    }

    // Reset button 3 pressed flag
    Recvdata.button3Pressed = false;  // Ensure button state reset is in the right place
  }

  // Timer code that runs independently of ESP-NOW
  if (timer0_Flag) {
    timer0_Flag = false;  // Reset the timer flag

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
  }
}
