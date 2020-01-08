#ifndef PID_CPP
#define PID_CPP

#include "Arduino.h"
#include <list>
#include "pwmfan.cpp"
#include "boschtempsensor.cpp"
#include "mahletempsensor.cpp"
#include "utils.h"

class FanPIDController{
  public:
    PWMFan *fan;
    MahleTempSensor *temperature_sensor;
    int derivative_range = 5;
    unsigned integral_range = 60;
    std::list<float> temperature_sensor_values;
    int temperature_sensor_values_pointer = 0;
    float target_temperature = 80;
    float proportional_ratio = 5;
    float integral_ratio = 5;
    float derivative_ratio = 5;

    FanPIDController(PWMFan *init_fan, MahleTempSensor *init_temperature_sensor){
      fan = init_fan;
      temperature_sensor = init_temperature_sensor;
    }

    void save_temperature(int temperature){
      temperature_sensor_values.push_front(temperature);
      if (temperature_sensor_values.size() > integral_range){
        temperature_sensor_values.pop_back();
      }
    }

    float get_temperature(){
      return temperature_sensor_values.front();
    }

    int get_target_speed(){
      float delta = get_temperature() - target_temperature;
      float proportional_part = proportional_ratio * delta;

      float integral_sum = 0;

      std::list<float>::iterator it;

      for (it = temperature_sensor_values.begin(); it != temperature_sensor_values.end(); ++it){
        if ((delta > 0 && *it < 0) || (delta < 0 && *it > 0))
          break;

        integral_sum += *it;
      }

      float integral_part = integral_ratio * integral_sum;

      it = temperature_sensor_values.begin();
      std::advance(it, derivative_range);

      float derivative_part = derivative_ratio * (temperature_sensor_values.front() - *it);

      float speed = proportional_part + integral_part + derivative_part;

      speed = max(speed, 0);
      speed = min(speed, 255);
      return speed;
    }

    void updateFanSpeed(){
      float temperature = temperature_sensor->getTemperature();
      save_temperature(temperature);
      int new_fan_speed = get_target_speed();
      fan->set_speed(new_fan_speed);
    }
};

#endif