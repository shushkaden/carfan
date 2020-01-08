#ifndef BOSCHTEMPSENSOR_CPP
#define BOSCHTEMPSENSOR_CPP

#include "Arduino.h"
#include "tempsensor.cpp"

class BoschTempSensor: public TempSensor{
  public:
    BoschTempSensor(int res, uint8_t arduino_pin) : TempSensor(res, arduino_pin){}
    int intervals = 3;
    float temperature_resistance_dependancy[3][5] =
    // resistance, a, b, c, d for formula 
    // y = ax^3 + bx^2 + cx + d where x is resistance and y is temperature
    {
      {620, -0.0000000008, 0.0000102086, -0.0460287738, 82.6502991079},
      {220, -0.0000006066, 0.0008729071, -0.478002516, 163.1704746474},
      {0, 0, 0.0038767203, -1.1928668347, 205.3246753247}
    };
};


#endif