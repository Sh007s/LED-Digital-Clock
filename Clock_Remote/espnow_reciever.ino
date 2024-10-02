#include <esp_now.h>
#include <WiFi.h>

// MAC Address of the sender
uint8_t senderAddress[] = {0xD8, 0xBC, 0x38, 0xF9, 0xC9, 0x40};

// Callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
 
  char message[250];
  memcpy(message, incomingData, len);
  message[len] = '\0';  // Null-terminate the string
 
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("From: ");
  Serial.println(macStr);
  Serial.print("Message: ");
  Serial.println(message);
  Serial.println();
}
 
void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);

  // Print MAC address
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, senderAddress, 6);
  peerInfo.channel = 0;  // Use the current channel
  peerInfo.encrypt = false;
  
  // Add peer        
  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus == ESP_OK) {
    Serial.println("Sender peer added successfully");
  } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    Serial.println("ESP-NOW not initialized");
  } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid argument");
  } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
    Serial.println("Peer list full");
  } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("Out of memory");
  } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
    Serial.println("Peer exists");
  } else {
    Serial.println("Not sure what happened");
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver initialized. Waiting for messages...");
}
 
void loop() {
  // Nothing to do here
}