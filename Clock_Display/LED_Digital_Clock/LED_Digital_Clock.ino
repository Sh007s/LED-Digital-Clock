#include <Wire.h>
#include "espnow.h"
#include "RTC.h"
#include "Timer0.h"
#include "WS2812.h"
#include "GIPO_PIN.h"
#include "DHT11_Sensor.h"
#include "TM1637_7Display.h"
#include "Dot_Matrix.h"

#define SHORT_PRESS_SEC 1000    // Duration for short press in milliseconds
#define MEDIUM_PRESS_MINS 2000  // Duration for medium press in milliseconds
#define LONG_PRESS_HOURS 3000   // Duration for long press in milliseconds

struct TimerState {
  bool displayDay;      // Flag to determine if the day should be displayed
  bool timerRunning;    // Flag to track if the timer is running
  int pressCount;       // Counter to track number of + presses
  int currentDayIndex;  // Start with Week[0] (Sunday)
  int hourCount;        // To track the hour count
  int lastSyncMinute;   // To track the last sync minute, initialized to -1
};

// Create an instance of the TimerState structure
TimerState timerState;

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;

void Task1(void *DotMatrix) {
  while (true) {
    // Serial.println("Task 1 is running ");
    timerState.hourCount = hours;

    timerState.displayDay = true;

    // If we've reached 24 presses (24 hours), move to the next day
    if (timerState.hourCount >= 24) {
      timerState.pressCount = 0;     // Reset the counter for the new day
      timerState.currentDayIndex++;  // Move to the next day in the Week array

      // If we've reached the end of the week, loop back to Sunday (Week[0])
      if (timerState.currentDayIndex >= 7) {
        timerState.currentDayIndex = 0;
      }
    }

    // Debugging: Show the number of presses and the current day index
    // Serial.print("hourcount: ");
    // Serial.println(hourcount);
    // Serial.print("Current day index: ");
    // Serial.println(currentDayIndex);

    // Display the current day if flag is set
    if (timerState.displayDay || timerState.hourCount < 24) {
      scrollLeft(Week[timerState.currentDayIndex], counT[timerState.currentDayIndex]);  // Display current day
      delay(1000);                                                                      // Simulate hourly display
      timerState.displayDay = false;                                                    // Reset flag

      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

void Task2(void *Temp_hum) {
  while (true) {
    //  Serial.println("Task 2 is running ");
    float temperature = gettemp();
    float humidity = gethum();

    //   Serial.printf("Humidity: %.2f%%  Temperature: %.2f°C\n", humidity, temperature);

    showDispaly(temperature);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    showDispaly(humidity);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
  Serial.begin(115200);      // Initialize Serial Monitor
  init_espnow();
  init_rtc();
  init_timer0();     // Initialized the timer0
  init_ledsec();     // Initialized the LED strips
  init_DHT11();      // Initialized the DHT11
  init_7dispaly();   // Initialized the TM1637 Dispaly
  init_DOT_setup();  // Initialize the MAX7219 module

  xTaskCreate(Task1, "Task 1", 10000, NULL, 1, &Task1Handle);
  xTaskCreate(Task2, "Task 2", 10000, NULL, 1, &Task2Handle);

  // Initialize the received data structure
  Recvdata = { false, false, false, false, false, false, 0 };
  timerState = { false, false, 0, 0, 0, -1 };
}

void loop() {

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
    if (timerState.timerRunning) {
      timerStop(timer);  // Stop the timer if it's running
      Serial.println("Timer Paused");
      timerState.timerRunning = false;
    }
  } else {
    if (!timerState.timerRunning) {
      timerStart(timer);  // Start the timer if it's not running
      Serial.println("Timer Resumed");
      timerState.timerRunning = true;
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
      displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);
    } else if (Recvdata.timermins) {
      mins = (mins + 1) % 60;  // Increment minutes
      Serial.printf("Minutes running: %d\n", mins);
      displayDigit(mins / 10, mins % 10, led2);
    } else if (Recvdata.timerhours) {
      hours = (hours % 12) + 1;  // Increment hours, cycling from 1 to 12
      Serial.printf("Hours running: %d\n", hours);
      displayDigit(hours / 10, hours % 10, led3);
    } else {
      Serial.println("No timer mode active");
    }
    // Reset button 3 pressed flag
    Recvdata.button3Pressed = false;  // Ensure button state reset is in the right place
  }

  // Check if timer0 has enabled
  if (timer0_Flag) {
    timer0_Flag = false;  // Clear flag
    timerISR();

    // Handle second rollover
    if (CountIsrAT >= 60) {
      CountIsrAT = 0;    // Reset seconds
      mins++;            // Increment minutes
      if (mins >= 60) {  // If minutes reach 60, reset and increment hours
        mins = 0;
        hours++;            // Increment hours
        if (hours >= 24) {  // Reset hours after 11:59:59 to 00:00:00
          hours = 0;
        }
      }
    }
    // Print the current time
    Serial.print("Timer Time: ");
    Serial.printf("%02d hours : %02d mins : %02d sec\n", hours, mins, CountIsrAT);

    displayDigit(hours / 10, hours % 10, led3);            // Display minutes on LED strip 3
    displayDigit(mins / 10, mins % 10, led2);              // Display minutes on LED strip 2
    displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);  // Display seconds on LED strip 1

    // Sync RTC with Timer 0 at the start of every minute
    DateTime now = rtc.now();
    if (now.minute() != timerState.lastSyncMinute) {
      sync_time_with_rtc();  // Use the new sync function
      timerState.lastSyncMinute = now.minute();
      Serial.println("Time synced with RTC");
    }
  }
}