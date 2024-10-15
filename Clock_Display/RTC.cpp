#include "RTC.h"  // Include the RTC header
#include "Timer0.h"

// Global variables used by the timer and RTC logic
RTC_DS1307 rtc;
int hours = 0, mins = 0;

// Initialize the RTC
void init_rtc() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);  // Stop execution if RTC isn't found
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC time to compile time if RTC lost power
  }

  // Read the time from the RTC and initialize variables
  DateTime now = rtc.now();
  hours = now.hour();
  mins = now.minute();
  CountIsrAT = now.second();  // Sync seconds with RTC at startup

  // Print the current time
  Serial.print("Initial RTC Time: ");
  Serial.print(hours);
  Serial.print(':');
  Serial.print(mins);
  Serial.print(':');
  Serial.println(CountIsrAT);
}

// Sync time with RTC
void sync_time_with_rtc() {
  DateTime now = rtc.now();
  hours = now.hour();
  mins = now.minute();
  CountIsrAT = now.second();

  Serial.print("RTC Synced Time: ");
  Serial.print(hours);
  Serial.print(':');
  Serial.print(mins);
  Serial.print(':');
  Serial.println(CountIsrAT);
}

// Check if the RTC lost power
bool is_rtc_power_lost() {
  return !rtc.isrunning();
}
