#include "espnow.h"

// MAC Address of the sender (adjust as necessary)
uint8_t senderAddress[] = { 0xD8, 0xBC, 0x38, 0xF9, 0xC9, 0x40 };

// Define the global variable for received data and the message flag
timer_setup Recvdata;
bool messageReceived = false;  // Flag to track message reception

// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(Recvdata)) {
    memcpy(&Recvdata, incomingData, sizeof(Recvdata));
    messageReceived = true;  // Mark message as received
  } else {
    Serial.println("Received data length mismatch");
  }
}

// Function to initialize ESP-NOW
void init_espnow() {
  WiFi.mode(WIFI_STA);  // Set WiFi mode to Station

  // Print MAC address of the receiver
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, senderAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus == ESP_OK) {
    Serial.println("Sender peer added successfully");
  } else {
    Serial.print("Failed to add peer: ");
    Serial.println(addStatus);
  }

  // Register the callback function for received data
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver initialized. Waiting for messages...");
}
