#include "DHT11_Sensor.h"
#include "TM1637_7Display.h"

// Create an instance of the DHTesp class
DHTesp dht;

void init_DHT11(void) {
  dht.setup(DHT_PIN, DHTesp::DHT11);
}

float gettemp() {
  float temp = dht.getTemperature();
  // Check if any readings failed
  if (isnan(temp)) {
    Serial.println(F("Failed to read temperature from DHT sensor!"));
    return 0.0;
  }
  return temp;
}

float gethum() {  // Removed the extra semicolon
  float hum = dht.getHumidity();
  if (isnan(hum)) {
    Serial.println(F("Failed to read humidity from DHT sensor!"));
    return 0.0;
  }
  return hum;
}
