#include "Button.h"
#include "espnow.h"

typedef struct time_setup {
  bool timerPaused;
  bool timersec;
  bool timermins;
  bool timerhours;
  bool button2Pressed;
  bool button3Pressed;
  unsigned long pressDuration;
} timer_setup;

timer_setup Senddata;

void setup() {
  Serial.begin(115200);
  init_espnow();
  init_Button();

  Senddata = {};  // Initialize Senddata structure
}

void loop() {
  button1.loop();
  button2.loop();
  button3.loop();

  // Handle button1 (toggle timer pause state)
  if (button1.isPressed()) {
    Senddata.timerPaused = !Senddata.timerPaused;
    esp_now_send(receiverAddress, (uint8_t *)&Senddata, sizeof(Senddata));
    Serial.println(Senddata.timerPaused ? "Timer paused" : "Timer resumed");
  }

  if (Senddata.timerPaused && button2.isPressed()) {
    Senddata.pressDuration = millis();
    Serial.println("Button 2 is pressed");
    Senddata.button2Pressed = true;
  }

  if (Senddata.timerPaused && button2.isReleased() && Senddata.button2Pressed == true) {
    Senddata.pressDuration = millis() - Senddata.pressDuration;
    Serial.println("Button 2 isReleased");
    Serial.println(Senddata.pressDuration);

    // Determine timer mode
    if (Senddata.pressDuration >= LONG_PRESS_HOURS) {
      Senddata.timerhours = true;
      Senddata.timermins = false;
      Senddata.timersec = false;
      Serial.println("Long press detected. Timer set to hours mode.");
    } else if (Senddata.pressDuration >= MEDIUM_PRESS_MINS) {
      Senddata.timermins = true;
      Senddata.timerhours = false;
      Senddata.timersec = false;
      Serial.println("Medium press detected. Timer set to minutes mode.");
    } else if (Senddata.pressDuration >= SHORT_PRESS_SEC) {
      Senddata.timersec = true;
      Senddata.timerhours = false;
      Senddata.timermins = false;
      Serial.println("Short press detected. Timer set to seconds mode.");
    } else {
      Senddata.timersec = false;
      Senddata.timerhours = false;
      Senddata.timermins = false;
      Serial.println("Button 2 press too short. No action taken.");
    }
    // Send updated status
    if (esp_now_send(receiverAddress, (uint8_t *)&Senddata, sizeof(Senddata)) != ESP_OK) {
      Serial.println("Error sending data via ESP-NOW");
    } else {
      Serial.println("Sent button press status successfully.");
    }
    Senddata.button2Pressed = false;
  }

  if (Senddata.timerPaused && button3.isPressed()) {
    Serial.println("Button 3 press");
  }
  if (Senddata.timerPaused && button3.isReleased()) {
    Serial.println("Button 3 Released");
    Senddata.button3Pressed = true;

    if (esp_now_send(receiverAddress, (uint8_t *)&Senddata, sizeof(Senddata)) != ESP_OK) {
      Serial.println("Error sending data via ESP-NOW");
    } else {
      Serial.println("Sent button press status successfully.");
    }
    Senddata.button3Pressed = false;
  }
}