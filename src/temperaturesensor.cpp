#ifndef TEMPERATURESENSOR_CPP
#define TEMPERATURESENSOR_CPP

#include "Arduino.h"

class TemperatureSensor{
  public:
    int voltage_delimiter_resistance;
    int sensor_voltage;
    float resistance;
    float sensor_temperature;
    uint8_t pin;

    TemperatureSensor(int res, uint8_t arduino_pin){
      voltage_delimiter_resistance = res;
      pin = arduino_pin;
    }

    float getTemperatureSquare(float a, float b, float c){
      // y = ax^2 + bx + c
      float temperature = c;
      temperature += resistance * b;
      temperature += resistance * resistance * a;

      return temperature;
    }

    float getTemperatureQube(float a, float b, float c, float d){
      String dataString = String("old class") + ":"
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

    virtual float getTemperature();

    void getResistance(){
      sensor_voltage = analogRead(pin);
      if (sensor_voltage == 0){sensor_voltage = 1;}

      resistance = 1023 - sensor_voltage;
      if (resistance == 0){resistance = 1;}
      
      resistance /= sensor_voltage;
      resistance *= voltage_delimiter_resistance;
    }
};

#endif