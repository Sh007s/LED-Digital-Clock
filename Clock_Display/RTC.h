// RTC.h
#ifndef RTC_H
#define RTC_H

#include <RTClib.h>  // Include RTClib for RTC functionality

#define SDA 21
#define SCL 22

extern int hours;
extern int mins;
extern RTC_DS1307 rtc;

void init_rtc();
void sync_time_with_rtc();  // You can create a function for syncing if needed
bool is_rtc_power_lost();         // Check if RTC lost power

#endif  // RTC_H
