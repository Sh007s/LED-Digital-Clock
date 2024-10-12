#ifndef ESPNOW_H
#define ESPNOW_H

#include <esp_now.h>
#include <WiFi.h>

// External declaration of the receiver's MAC address
extern uint8_t receiverAddress[];

// Callback function for when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

// Function to initialize ESP-NOW
void init_espnow();

#endif // ESPNOW_H
