#include"Timer0.h"
#include "WS2812.h"

// define the LED pin to Toggle
#define LED_PIN 2
volatile uint32_t second = 0;

void setup() {
  
  pinMode(LED_PIN, OUTPUT);    // Set LED pin as output
  Serial.begin(115200);       // Initialize Serial Monitor
  init_timer0();              //initized the timer0
  init_ledsec();
  }

void loop() {
  //Check if timer0 has enable
  if(timer0_Flag)
  {
    // Clear the flag
    timer0_Flag = false;
    //Reset CountIsrAT after 60 seconds
    if(CountIsrAT >= 60)
    {
      CountIsrAT = 0;
    }
    Serial.printf("sec : ");
    Serial.println(CountIsrAT);

    displayDigit(CountIsrAT / 10, CountIsrAT % 10);
    second = CountIsrAT % 10;
    Serial.print("Count : ");
    Serial.println(second);
  }
}