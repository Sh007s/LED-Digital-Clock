#ifndef GPIO_PIN_H
#define GPIO_PIN_H

// define the LED pin to Toggle
#define LED_PIN 2

// define the LED strip 1, 2, 3 pins :-> Change the LED_PIN1,2 pins to 12,25 pins
#define LED_PIN1 5     
#define LED_PIN2 4     
#define LED_PIN3 15    

// define the DHT11 Sensor data pin 
#define DHT_PIN 26

// Define the Dot matrix pins
#define DIN_PIN 22  // Data In (DIN) /* Change the DIN_PIN to 27  */
#define CS_PIN  17  // Chip Select (CS)
#define CLK_PIN 16  // Clock (CLK)

// define the TM1637_Dispaly pin
#define CLK 18
#define DIO 23
/*
// define the DS1307 RTC module pin
#define CLK 21
#define DIO 22
//Add the pin for NRF Module, Ds1307 RTC module 16-bit ws2812 led strip
*/
#endif // GPIO_PIN_H