#ifndef SENSOR_H
#define SENSOR_H

// #include <OneWire.h>
#include <DallasTemperature.h>

float valeurDS18B20(DallasTemperature sensor);

float valeurACS712(int pin);

#endif //SENSOR_H