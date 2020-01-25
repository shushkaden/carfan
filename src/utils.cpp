#include "Arduino.h"
#include <ArduinoSTL.h>

int max(int a,int b) {return ((a)>(b)?(a):(b)); }
int min(int a,int b) {return ((a)<(b)?(a):(b)); }

float max_float(float a,float b) {return ((a)>(b)?(a):(b)); }
float min_float(float a,float b) {return ((a)<(b)?(a):(b)); }

String leadingZero(int number){
  String leadingZeroNumber = String(number);
  if (number < 10){
    leadingZeroNumber = "0" + leadingZeroNumber;
  }

  return leadingZeroNumber;
}