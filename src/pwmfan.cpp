#ifndef PWMFAN_CPP
#define PWMFAN_CPP

#include "Arduino.h"
#include "utils.h"

class PWMFan{
  public:
    uint8_t pin;
    int* timer;
    int timer_time;
    int current_speed;
    int min_speed = 10;
    int turn_on_speed = 13;
    int turn_off_speed = 7;
    int starting_speed = 30;
    int starting_time = 1000; // milliseconds / 2
    bool is_running;
    bool is_starting;

    PWMFan(uint8_t arduino_pin, int &fan_timer){
      pin = arduino_pin;
      timer = &fan_timer;
      is_running = false;
      is_starting = false;
    }

    void start(){
      is_running = true;
      is_starting = true;
      timer_time = *timer;
      _set_speed(starting_speed);
    }

    void stop(){
      is_running = false;
      _set_speed(0);
    }

    bool can_change_speed(){
      if (!is_starting){
        return true;
      }

      if (*timer > timer_time + starting_time){
        is_starting = false;
      }

      return !is_starting;
    }

    void set_speed(int new_speed){
      if (can_change_speed()){
        return;
      }

      if (is_running == false and new_speed >= turn_on_speed){
        start();
        return;
      }
      if (is_running == true and new_speed < turn_off_speed){
        stop();
        return;
      }

      new_speed = max(new_speed, min_speed);
      _set_speed(new_speed);
    }

    void _set_speed(int new_speed){
      current_speed = new_speed;
      analogWrite(pin, new_speed);
    }
};

#endif