#ifndef MAHLETEMPSENSOR_CPP
#define MAHLETEMPSENSOR_CPP

#include "Arduino.h"
#include "tempsensor.cpp"

class MahleTempSensor: public TempSensor{
  public:
    MahleTempSensor(int res, uint8_t arduino_pin) : TempSensor(res, arduino_pin){}
    int intervals = 3;
    float temperature_resistance_dependancy[3][5] =
    // resistance, a, b, c, d for formula 
    // y = ax^3 + bx^2 + cx + d where x is resistance and y is temperature
    {
      {660, -0.0000000004, 0.0000056369, -0.0326492099, 81.9137133308},
      {150, -0.0000002749, 0.0004962523, -0.3409863073, 153.9039721089},
      {0, -0.0, 0.0007785764, -0.4506398060, 163.4393979597}
    };
};


#endif