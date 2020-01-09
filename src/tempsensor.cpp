#ifndef TEMPSENSOR_CPP
#define TEMPSENSOR_CPP

#include "Arduino.h"

class TempSensor{
  public:
    int voltage_delimiter_resistance;
    int sensor_voltage;
    float resistance;
    float sensor_temperature;
    uint8_t pin;
    int intervals = 4;
    float temperature_resistance_dependancy[4][5] =
    // resistance, a, b, c, d for formula 
    // y = ax^3 + bx^2 + cx + d where x is resistance and y is temperature
    {
      {951, -0.0000000014, 0.0000125856, -0.045017722, 86.33353376},
      {513, 0.0000000019, 0.0000243565, -0.0810552502, 107.2423883},
      {185, -0.0000004139, 0.000633673, -0.3878451016, 160.4001628},
      {0, -0.0000108497, 0.0059138217, -1.287152349, 212.1403592}
    };

    TempSensor(int res, uint8_t arduino_pin){
      voltage_delimiter_resistance = res;
      pin = arduino_pin;
    }

    float getTemperatureQube(float a, float b, float c, float d){
      // y = ax^3 + bx^2 + cx + d
      float temperature = d;
      temperature += resistance * c;
      temperature += resistance * resistance * b;
      temperature += resistance * resistance * resistance * a;

      return temperature;
    }

    void getResistance(){
      sensor_voltage = analogRead(pin);
      if (sensor_voltage == 0){sensor_voltage = 1;}

      resistance = 1023 - sensor_voltage;
      if (resistance == 0){resistance = 1;}
      
      resistance /= sensor_voltage;
      resistance *= voltage_delimiter_resistance;
    }

    float getTemperature(){
      getResistance();
      int interval = 0;
      while (resistance <= temperature_resistance_dependancy[interval][0]){interval++;}
      return getTemperatureQube(
        temperature_resistance_dependancy[interval][1],
        temperature_resistance_dependancy[interval][2],
        temperature_resistance_dependancy[interval][3],
        temperature_resistance_dependancy[interval][4]
      );
    }
};

#endif