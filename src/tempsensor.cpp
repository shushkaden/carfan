#ifndef TEMPSENSOR_CPP
#define TEMPSENSOR_CPP

#include "Arduino.h"
#include <list>
#include "utils.h"

class TempSensor{
  public:
    int voltage_delimiter_resistance;
    int sensor_voltage;
    float resistance;
    float sensor_temperature;
    uint8_t pin;
    unsigned int values_range = 6;
    std::list<float> temperature_values;
    std::list<float> delta_values;
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

    float positive_temperature_delta_dependancy[7][2] =
    {
      {1.03, 7.47},
      {0.69, 5.80},
      {0.53, 5.54},      
      {0.29, 4.34},
      {0.21, 3.05},
      {0.10, 1.00},
      {0.00, 0.00},
    };

    float negative_temperature_delta_dependancy[4][2] =
    {
      {-0.16, -4.14},
      {-0.11, -2.50},
      {-0.06, -0.65},
      {0.00, 0.00},
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

    void rereadResistance(){
      sensor_voltage = analogRead(pin);
      if (sensor_voltage == 0){sensor_voltage = 1;}

      resistance = 1023 - sensor_voltage;
      if (resistance == 0){resistance = 1;}
      
      resistance /= sensor_voltage;
      resistance *= voltage_delimiter_resistance;
    }

    float getRawTemperature(){
      int interval = 0;
      while (resistance <= temperature_resistance_dependancy[interval][0]){interval++;}
      return getTemperatureQube(
        temperature_resistance_dependancy[interval][1],
        temperature_resistance_dependancy[interval][2],
        temperature_resistance_dependancy[interval][3],
        temperature_resistance_dependancy[interval][4]
      );
    }

    float getIncrementByPositiveDelta(float delta){
      float increment = 0;
      int interval = 0;
      while (delta < positive_temperature_delta_dependancy[interval][0]){interval++;}
      
      if (interval == 0){
        increment = delta / positive_temperature_delta_dependancy[interval][0] * positive_temperature_delta_dependancy[interval][1];
      } else {
        float multiplier = (positive_temperature_delta_dependancy[interval][1] - positive_temperature_delta_dependancy[interval-1][1]) / 
          (positive_temperature_delta_dependancy[interval][0] - positive_temperature_delta_dependancy[interval-1][0]);
        increment = delta * multiplier;
      }

      return increment;
    }

    float getIncrementByNegativeDelta(float delta){
      float increment = 0;
      int interval = 0;
      while (delta > negative_temperature_delta_dependancy[interval][0]){interval++;}
      
      if (interval == 0){
        increment = delta / negative_temperature_delta_dependancy[interval][0] * negative_temperature_delta_dependancy[interval][1];
      } else {
        float multiplier = (negative_temperature_delta_dependancy[interval][1] - negative_temperature_delta_dependancy[interval-1][1]) / 
          (negative_temperature_delta_dependancy[interval][0] - negative_temperature_delta_dependancy[interval-1][0]);
        increment = delta * multiplier;
      }

      return increment;
    }

    float getIncrementByDelta(float delta){
      if (delta >= 0){
        return getIncrementByPositiveDelta(delta);
      }
      return getIncrementByNegativeDelta(delta);
    }

    float getTemperature(){
      float delta = delta_values.front();
      float increment = getIncrementByDelta(delta);
      float countedTemperature = temperature_values.front() + increment;

      return countedTemperature;
    }

    float getSmoothTemperature(){
      float oldTemprature = temperature_values.front();
      float newTemprature = getRawTemperature();
      float diff = newTemprature - oldTemprature;
      float smoothTemperature = oldTemprature + diff * min(max((abs(diff)/2), 0.05), 1);

      return smoothTemperature;
    }

    float getSmoothDelta(){
      float oldDelta = delta_values.front();
      float newDelta = temperature_values.front() - temperature_values.back();
      float diff = newDelta - oldDelta;
      float smoothDelta = oldDelta + diff * min(max((abs(diff)), 0.1), 1);

      return smoothDelta;
    }

    void rereadTemperature(){
      float temperature;

      if (temperature_values.size() == 0){
        temperature = getRawTemperature();
      } else {
        temperature = getSmoothTemperature();
      }
      
      temperature_values.push_front(temperature);

      if (temperature_values.size() > values_range){
        temperature_values.pop_back();
      }
    }

    void rereadDelta(){
      float delta;

      if (delta_values.size() != values_range){
        delta = 0;
      } else {
        delta = getSmoothDelta();
      }

      if (abs(delta) <= 0.01) {delta = 0.0;}
      
      delta_values.push_front(delta);

      if (delta_values.size() > values_range){
        delta_values.pop_back();
      }
    }

    void updateData(){
      rereadResistance();
      rereadTemperature();
      rereadDelta();
    }
};

#endif