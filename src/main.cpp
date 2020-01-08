// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <max6675.h> 
#include <ArduinoSTL.h>
#include <list>
#include "utils.h"
#include "tempsensor.cpp"
#include "mahletempsensor.cpp"
#include "temperaturesensor.cpp"
#include "mahletemperaturesensor.cpp"
#include "pid.cpp"
#include "pwmfan.cpp"

#define sensor1_pin A0
#define sensor2_pin A1
#define delimiter_value 266

#define thermoDO 7
#define thermoCS 6
#define thermoCLK 5

MAX6675 thermocouple;

RTC_DS1307 rtc;

const int chipSelect = 4;
File root;
File logFile;

int sensors_read_period = 500; // milliseconds / 2
int sensors_read_timer = 0;
int common_timer = 0;
bool sensors_read_flag = false;

String nowString(){
  DateTime now = rtc.now();
  String nowString = String()
    + leadingZero(now.year()) + "-"
    + leadingZero(now.month()) + "-"
    + leadingZero(now.day()) + " "
    + leadingZero(now.hour()) + ":"
    + leadingZero(now.minute()) + ":"
    + leadingZero(now.second());
  
  return nowString;
}

// BoschTemperatureSensor sensor1 = BoschTemperatureSensor(delimiter_value, sensor1_pin);
MahleTemperatureSensor sensor2 = MahleTemperatureSensor(delimiter_value, sensor2_pin);
// FIX THIS
// BoschTempSensor newsensor1 = BoschTempSensor(delimiter_value, sensor1_pin);
MahleTempSensor newsensor2 = MahleTempSensor(delimiter_value, sensor2_pin);

void initHardware () {
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  thermocouple.begin(thermoCLK, thermoCS, thermoDO);
}


void sensors_read(){
  sensors_read_flag = false;
  // float sensor1_temperature = sensor1.getTemperature();
  // float sensor1_resistance = sensor1.resistance;
  float sensor2_temperature = sensor2.getTemperature();
  float sensor2_resistance = sensor2.resistance;

  // float newsensor1_temperature = newsensor1.getTemperature();
  // float newsensor1_resistance = newsensor1.resistance;
  float newsensor2_temperature = newsensor2.getTemperature();
  float newsensor2_resistance = newsensor2.resistance;

  float thermocouple_temperature = thermocouple.readCelsius();

  String dataString = nowString() + ","
    // + sensor1_temperature + ","
    // + sensor1_resistance + ","
    + sensor2_temperature + ","
    + sensor2_resistance + ","
    // + newsensor1_temperature + ","
    // + newsensor1_resistance + ","
    + newsensor2_temperature + ","
    + newsensor2_resistance + ","
    + thermocouple_temperature + ",";

  if (logFile) {
    logFile.println(dataString);
    logFile.flush();
  }

  Serial.println(dataString);

  // String debug = String("") + sensor1.sensor_voltage + ' ' + sensor1.resistance + ' ' + sensor1_temperature;
  // Serial.println(debug);
}

void setup () {
  initHardware();

  DateTime now = rtc.now();
  String logFileNameString = leadingZero(now.hour()) + "-"
                           + leadingZero(now.minute()) + "-"
                           + leadingZero(now.second()) + ".csv";
  // int logFileNameLength = 24;
  
  // char logFileName[logFileNameLength];
  // logFileNameString.toCharArray(logFileName, logFileNameLength);
  // Serial.print("File name: ");
  // for (size_t i = 0; i < logFileNameLength; i++)
  // {
  //   Serial.print(logFileName[i]);
  // }
  // Serial.println();

  // Serial.println("length: " + logFileNameLength);

  // Open up the file we're going to log to!
  logFile = SD.open(logFileNameString.c_str() , FILE_WRITE);
  
  if (! logFile) {
    Serial.println("error opening log file");
    // Wait forever since we cant write data
    // while (1) ;
  }
  
  String logFileHeader = String()
    + "time,"
    + "s2_t,"
    + "s2_r,"
    + "news2_t,"
    + "news2_r,"
    // + "ns1_t,"
    // + "ns1_r,"
    // + "ns2_t,"
    // + "ns2_r,"
    + "therma,";

  if (logFile) {
    logFile.println(logFileHeader);
    logFile.flush();
  }

  Serial.println(logFileHeader);

  // sensor1 = BoschTemperatureSensor(delimiter_value, sensor1_pin);
  // sensor2 = MahleTemperatureSensor(delimiter_value, sensor2_pin);
  // newsensor1 = BoschTempSensor(delimiter_value, sensor1_pin);
  // newsensor2 = MahleTempSensor(delimiter_value, sensor2_pin);

  MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

  /**** настройка прерывания по таймеру каждую 0,001 сек (вызов функции ISR (TIMER0_COMPA_vect)) ****/
  cli();
  TCCR2A = 0; // установить регистры в 0
  // TCCR0B = 0;
  TCCR2A |= (1 << WGM21);            //сброс при совпадении
  OCR2A = 0xF9;                         //начало отсчета до переполнения (249)
  TIMSK2 |= (1 << OCIE2A);             //разрешить прерывание при совпадении с регистром А
  TCCR2B |= (1 << CS22) | (1 << CS20); //установить делитель частоты на 64 
  sei();                               //разрешить прерывания

}

ISR (TIMER2_COMPA_vect)
{ 
  common_timer++;

  sensors_read_timer++;
  if (sensors_read_timer == sensors_read_period){
    sensors_read_timer = 0;
    sensors_read_flag = true;
  }
}

void loop () {
  if (sensors_read_flag){
    sensors_read();
  }
}
