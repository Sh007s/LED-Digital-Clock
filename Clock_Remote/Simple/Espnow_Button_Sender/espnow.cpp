#include "espnow.h"

// MAC Address of the receiver
uint8_t receiverAddress[] = { 0x10, 0x06, 0x1C, 0xF4, 0x52, 0x5C };

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(mac_addr != NULL){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }
  else
  {
    Serial.println("Error: MAC address is NULL");
  }
}


void init_espnow() {
  WiFi.mode(WIFI_STA);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

 // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW initialization successful");
  Serial.println("Sender initialized. Ready to send messages.");
}
