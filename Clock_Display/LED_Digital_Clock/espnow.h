#ifndef ESPNOW_H
#define ESPNOW_H

#include <esp_now.h>
#include <WiFi.h>

// Define the structure to hold received data
typedef struct time_setup {
  bool timerPaused;
  bool timersec;
  bool timermins;
  bool timerhours;
  bool button2Pressed;
  bool button3Pressed;  // Added button3Pressed
  unsigned long pressDuration;
} timer_setup;

// Declare the external variables
extern timer_setup Recvdata;     // Structure for received data
extern uint8_t senderAddress[];  // Sender's MAC address
extern bool messageReceived;     // Flag to track message reception

// Function declarations
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void init_espnow();  // Function to initialize ESP-NOW

#endif  // ESPNOW_H
