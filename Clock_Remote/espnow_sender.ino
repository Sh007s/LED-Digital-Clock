#include <esp_now.h>
#include <WiFi.h>

// MAC Address of the receiver
uint8_t receiverAddress[] = { 0x10, 0x06, 0x1C, 0xF4, 0xF6, 0x14 };

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  // Use the current channel
  peerInfo.encrypt = false;

  // Add peer
  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus == ESP_OK) {
    Serial.println("Peer added successfully");
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

  Serial.println("Sender initialized. Ready to send messages.");
}
//int count = 1;
void loop() {
  static int count = 0;
  static String msg = "";

    // Convert the message to a C string using c_str() and get its length
    if (Serial.available() > 0 || count == 1) {
    //  const char *message = "Hello from ESP32 Sender!";
    msg = Serial.readStringUntil('\n');

    if (msg.length() > 0) {
     // Convert the message to a C string using c_str() and get its length
      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg.c_str(), msg.length());

      if (result == ESP_OK) {
        Serial.println("Message sent successfully");
      } else {
        Serial.println("Error sending the message");
      }
      count = 0;    // Prevent multiple sends until reset
    }
  }
  // Reset count after 5 seconds to allow another message to be sent
  if (count == 0 && millis() % 1000 == 0) {
    count = 1;
    Serial.println("Ready to send another message. Please enter a new message:");
  }
}