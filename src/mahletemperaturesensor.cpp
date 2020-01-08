#ifndef MAHLETEMPERATURESENSOR_CPP
#define MAHLETEMPERATURESENSOR_CPP

#include "Arduino.h"
#include "temperaturesensor.cpp"

class MahleTemperatureSensor: public TemperatureSensor{
  public:
    MahleTemperatureSensor(int res, uint8_t arduino_pin) : TemperatureSensor(res, arduino_pin){}

    float getHighTemp(){
      return getTemperatureSquare(
        0.0007785764,
        -0.4506398060,
        163.4393979597);
    }

    float getMidTemp(){
      return getTemperatureQube(
        -0.0000002749,
        0.0004962523,
        -0.3409863073,
        153.9039721089);
    }

    float getLowTemp(){
      return getTemperatureQube(
        -0.0000000004,
        0.0000056369,
        -0.0326492099,
        81.9137133308);
    }

    float getTemperature(){
      getResistance();
      if (resistance < 150){
        return getHighTemp();
      } else if (resistance < 660){
        return getMidTemp();
      } else {
        return getLowTemp();
      }
    }
};


#endif