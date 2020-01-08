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
    int intervals = 5;
    float temperature_resistance_dependancy[5][5] =
    // resistance, a, b, c, d for formula 
    // y = ax^3 + bx^2 + cx + d where x is resistance and y is temperature
    {
      {660, 2, 3, 4, 5},
      {300, 2, 3, 4, 5},
      {200, 2, 3, 4, 5},
      {100, 2, 3, 4, 5},
      {0, 2, 3, 4, 5}
    };

    TempSensor(int res, uint8_t arduino_pin){
      voltage_delimiter_resistance = res;
      pin = arduino_pin;
    }

    float getTemperatureQube(float a, float b, float c, float d){
      String dataString = String("new class") + ":"
    // + sensor1_temperature + ","
    // + sensor1_resistance + ","
    + a + ","
    + b + ","
    // + newsensor1_temperature + ","
    // + newsensor1_resistance + ","
    + c + ","
    + d + ",";
      Serial.println(dataString);
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