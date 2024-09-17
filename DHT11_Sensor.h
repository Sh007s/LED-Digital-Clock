#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

#include <DHTesp.h>
#include <Adafruit_Sensor.h>
#include "GIPO_PIN.h"

void init_DHT11(void);
float gettemp();
float gethum();

#endif  // DHT11_SENSOR_H
