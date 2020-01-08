#ifndef BOSCHTEMPERATURESENSOR_CPP
#define BOSCHTEMPERATURESENSOR_CPP

#include "Arduino.h"
#include "temperaturesensor.cpp"

class BoschTemperatureSensor: public TemperatureSensor{
  public:
    BoschTemperatureSensor(int res, uint8_t arduino_pin) : TemperatureSensor(res, arduino_pin){}

    float getHighTemp(){
      return getTemperatureSquare(
        0.0038767203,
        -1.1928668347,
        205.3246753247);
    }

    float getMidTemp(){
      return getTemperatureQube(
        -0.0000006066,
        0.0008729071,
        -0.478002516,
        163.1704746474);
    }

    float getLowTemp(){
      return getTemperatureQube(
        -0.0000000008,
        0.0000102086,
        -0.0460287738,
        82.6502991079);
    }

    float getTemperature(){
      getResistance();
      if (resistance < 110){
        return getHighTemp();
      } else if (resistance < 620){
        return getMidTemp();
      } else {
        return getLowTemp();
      }
    }
};


#endif