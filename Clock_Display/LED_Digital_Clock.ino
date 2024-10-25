#include <Wire.h>
#include <RTClib.h>  // RTC library for DS1307/DS3231 or similar RTC modules
#include <EEPROM.h>
#include "Timer0.h"
#include "espnow.h"
#include "WS2812.h"
#include "GIPO_PIN.h"
#include "DHT11_Sensor.h"
#include "TM1637_7Display.h"
#include "Dot_Matrix.h"

#define SHORT_PRESS_SEC 500
#define MEDIUM_PRESS_MINS 1000
#define LONG_PRESS_HOURS 2000
#define PRESS_FOR_WEEKDAY 3000
#define EEPROM_SIZE 512  // Define the size of EEPROM

RTC_DS3231 rtc;  // Create an RTC object

struct TimerState {
  bool displayDay;
  bool timerRunning;
  static int currentDayIndex;
  int hourCount;
  int minscount;
  int seccount;
  int lastSyncMinute;
};

int TimerState::currentDayIndex = 0;  // Definition of static variable

// Timer and ESP32 time variables
int hours = 0, mins = 0;
TimerState timerState;

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;

void init_eeprom() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to Initialize EEPROM");
    return;
  }

  // Read the saved day index from EEPROM at address 0
  int savedDayIndex = EEPROM.read(0);

  // Check if saved value is valid (in the range 0-6 for the week days)
  if (savedDayIndex >= 0 && savedDayIndex <= 6) {
    timerState.currentDayIndex = savedDayIndex;
  } else {
    timerState.currentDayIndex = 0;  // Default to Sunday (0) if invalid
  }

  Serial.printf("Restored Day Index: %d\n", timerState.currentDayIndex);
}

void storeDayIndex() {
  EEPROM.write(0, timerState.currentDayIndex);  // Write to EEPROM at address 0
  EEPROM.commit();                              // Commit changes
  Serial.printf("Day index %d saved to EEPROM.\n", timerState.currentDayIndex);
}
// RTC initialization
void init_rtc() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// Sync ESP32 time with RTC
void sync_time_with_rtc() {
  DateTime now = rtc.now();
  hours = now.hour();
  mins = now.minute();
  CountIsrAT = now.second();
}

void adjust_rtc(int H, int M, int S);

void Task1(void *DotMatrix) {
  while (true) {
    // Assuming 'hours', 'mins', and 'CountIsrAT' are updated elsewhere based on real-time
    timerState.hourCount = hours;  // Update hourCount from external source
    timerState.displayDay = true;  // Set flag to display the day

    // Print current hour and week information for debugging
    // Serial.printf("Hourcount :  %d\n", timerState.hourCount);
    // Serial.printf("Week Number :  %d\n", timerState.currentDayIndex);
    // Display the day if displayDay flag is set or hourCount is less than 24
    if (timerState.displayDay || timerState.hourCount < 24) {
      scrollLeft(Week[timerState.currentDayIndex], counT[timerState.currentDayIndex]);  // Scroll the current day on the dot matrix
      timerState.displayDay = false;                                                    // Reset display flag
    }

    // Suspend the task for 1 second to allow the system to breathe
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 second delay to let the task wait
  }
}

// Task 2: Handle DHT11 Sensor Data
void Task2(void *Temp_hum) {
  while (true) {
    float temperature = gettemp();
    float humidity = gethum();

    showDispaly(temperature);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    showDispaly(humidity);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void DayCheckTask(void *parameter) {
  while (true) {
    // Check if it's a new day (00:00:00)
    if (hours == 0 && mins == 0 && CountIsrAT == 0) {
      Serial.println("Incrementing day index...");

      timerState.hourCount = 0;      // Reset hour count for the new day
      timerState.currentDayIndex++;  // Move to the next day

      if (timerState.currentDayIndex >= 7) {
        timerState.currentDayIndex = 0;  // Loop back to day 0 after a week
      }

      // Debug output
      Serial.print("Updated Week Number: ");
      Serial.println(timerState.currentDayIndex);

      storeDayIndex();  // Store the updated day index
    }

    adjust_rtc(hours, mins, CountIsrAT);  // Ensure RTC is up to date

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for 1 second to check every second
  }
}

// Setup function
void setup() {
  Serial.begin(115200);

  // Initialize various modules
  init_espnow();
  init_eeprom();
  init_rtc();
  init_timer0();
  init_ledsec();
  init_DHT11();
  init_7dispaly();
  init_DOT_setup();

  xTaskCreate(Task1, "Task 1", 4096, NULL, 1, &Task1Handle);
  xTaskCreate(Task2, "Task 2", 4096, NULL, 1, &Task2Handle);
  xTaskCreate(DayCheckTask, "DayCheckTask", 4096, NULL, 1, &Task3Handle);

  // Initial RTC sync
  sync_time_with_rtc();

  Recvdata = { false, false, false, false, false, false, false, 0 };
  timerState = { false, false, 0, 0, -1 };
}

// Main loop
void loop() {
  // Handle ESPNOW message
  if (messageReceived) {
    Serial.print("Timer paush: ");
    Serial.println(Recvdata.timerPaused);
    Serial.print("Seconds running: ");
    Serial.println(Recvdata.timersec);
    Serial.print("Minutes running: ");
    Serial.println(Recvdata.timermins);
    Serial.print("Hours running: ");
    Serial.println(Recvdata.timerhours);

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

  if (Recvdata.timerPaused && Recvdata.button2Pressed) {
    // Reset all flags first
    Recvdata.weekday = Recvdata.timerhours = Recvdata.timermins = Recvdata.timersec = false;

    // Set the appropriate flag based on press duration
    if (Recvdata.pressDuration >= PRESS_FOR_WEEKDAY) {
      Recvdata.weekday = true;
    } else if (Recvdata.pressDuration >= LONG_PRESS_HOURS) {
      Recvdata.timerhours = true;
    } else if (Recvdata.pressDuration >= MEDIUM_PRESS_MINS) {
      Recvdata.timermins = true;
    } else if (Recvdata.pressDuration >= SHORT_PRESS_SEC) {
      Recvdata.timersec = true;
    }

    Recvdata.button2Pressed = false;
  }
  // Increment corresponding timer values
  if (Recvdata.timerPaused && Recvdata.button3Pressed) {
    if (Recvdata.timersec) {
      CountIsrAT = (CountIsrAT + 1) % 60;
      displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);
    } else if (Recvdata.timermins) {
      mins = (mins + 1) % 60;
      displayDigit(mins / 10, mins % 10, led2);
    } else if (Recvdata.timerhours) {
      hours = (hours + 1) % 24;
      displayDigit(hours / 10, hours % 10, led3);
    } else if (Recvdata.weekday) {
      timerState.currentDayIndex++;
      if (timerState.currentDayIndex >= 7) {
        timerState.currentDayIndex = 0;
        Serial.printf("Day num :  %d\n", timerState.currentDayIndex);
      }
    }
    Recvdata.button3Pressed = false;
  }

  // Handle 1-second timer updates
  if (timer0_Flag) {
    timer0_Flag = false;

    if (CountIsrAT >= 60) {
      CountIsrAT = 0;
      mins++;
      if (mins >= 60) {
        mins = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }

    // Print the time tracked by the ESP32 (based on Timer 0)
    Serial.print("Timer Time: ");
    Serial.print(hours);
    Serial.print(':');
    Serial.print(mins);
    Serial.print(':');
    if (CountIsrAT < 10) {
      Serial.print('0');
    }
    Serial.println(CountIsrAT);

    displayDigit(hours / 10, hours % 10, led3);
    displayDigit(mins / 10, mins % 10, led2);
    displayDigit(CountIsrAT / 10, CountIsrAT % 10, led1);
  }
}

void adjust_rtc(int H, int M, int S) {
  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();

  rtc.adjust(DateTime(year, month, day, H, M, S));
 // Serial.println("RTC time adjusted!");
}