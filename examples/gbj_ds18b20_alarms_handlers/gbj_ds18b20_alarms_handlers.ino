/*
  NAME:
  Demonstrate DS18B20 temperature sensors alarm signalling by alarm handlers.

  DESCRIPTION:
  The sketch launches alarm handlers for sensors with an active alarm.
  The sketch provides the configuration part for setting alarm temperatures
  for better experimenting.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_ds18b20.h"

#define SKETCH "GBJ_DS18B20_ALARMS_HANDLERS 1.0.0"

const unsigned int PERIOD_MEASURE = 3000; // Miliseconds between measurements
const unsigned char PIN_DS18B20 = 4; // Pin for one-wire bus

void alarmHandlerLow();
void alarmHandlerHigh();
gbj_ds18b20 ds = gbj_ds18b20(PIN_DS18B20, alarmHandlerLow, alarmHandlerHigh);
gbj_ds18b20::Address address;
gbj_ds18b20::Sernum sernum;
char buffer[50];
uint8_t deviceNum;

String textAddress(gbj_ds18b20::Address address)
{
  String text = "";
  char data[3];
  for (byte i = 0; i < gbj_ds18b20::ADDRESS_LEN; i++)
  {
    if (i)
      text += "-";
    sprintf(data, "%02X", address[i]);
    text += data;
  }
  return text;
}

String textSernum(gbj_ds18b20::Sernum sernum)
{
  String text = "";
  char data[3];
  for (byte i = 0; i < gbj_ds18b20::SERNUM_LEN; i++)
  {
    if (i)
      text += ":";
    sprintf(data, "%02X", sernum[i]);
    text += data;
  }
  return text;
}

void errorHandler()
{
  switch (ds.getLastResult())
  {
    case gbj_ds18b20::SUCCESS:
      Serial.println("SUCCESS");
      break;

    case gbj_ds18b20::END_OF_LIST:
      Serial.println("END_OF_LIST");
      break;

    case gbj_ds18b20::ERROR_NO_DEVICE:
      Serial.println("ERROR_NO_DEVICE");
      break;

    case gbj_ds18b20::ERROR_CRC_ADDRESS:
      Serial.println("ERROR_CRC_ADDRESS");
      break;

    case gbj_ds18b20::ERROR_CRC_SCRATCHPAD:
      Serial.println("ERROR_CRC_SCRATCHPAD");
      break;

    case gbj_ds18b20::ERROR_NO_ALARM:
      Serial.println("ERROR_NO_ALARM");
      break;

    case gbj_ds18b20::ERROR_ALARM_LOW:
      Serial.println("ERROR_ALARM_LOW");
      break;

    case gbj_ds18b20::ERROR_ALARM_HIGH:
      Serial.println("ERROR_ALARM_HIGH");
      break;

    default:
      Serial.println("Unknown error");
      break;
  }
}

void alarmHandlerLow()
{
  Serial.println("Id: " + String(ds.getId()));
  Serial.println("Alarm Low: " + String(ds.getAlarmLow()) + " 'C");
  Serial.println("Temperature: " + String(ds.getTemperature(), 4) + " 'C");
  Serial.println("---");
}

void alarmHandlerHigh()
{
  Serial.println("Id: " + String(ds.getId()));
  Serial.println("Alarm High: " + String(ds.getAlarmHigh()) + " 'C");
  Serial.println("Temperature: " + String(ds.getTemperature(), 4) + " 'C");
  Serial.println("---");
}

void setup()
{
  Serial.begin(9600);
  Serial.println(); // Some sernum monitors display garbage at the begining
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(gbj_ds18b20::VERSION);
  Serial.println("---");
  Serial.println("One-wire statistics");
  Serial.println("Powered: " +
                 String(ds.isPowerParasite() ? "Parasite" : "External"));
  Serial.println("Devices: " + String(ds.getDevices()));
  Serial.println("Sensors: " + String(ds.getSensors()));
  Serial.println("---");
  deviceNum = 0;
  while (ds.isSuccess(ds.sensors()))
  {
    // Setup sensor
    switch (ds.getId())
    {
      case 0x86 /* Top sensor */:
        ds.cacheResolution_10bits();
        ds.cacheAlarmLow(19);
        ds.cacheAlarmHigh(27);
        break;

      case 0x6c /* Bottom sensor */:
        ds.cacheResolution_9bits();
        ds.cacheAlarmLow(30);
        ds.cacheAlarmHigh(35);
        break;

      default:
        ds.cacheResolutionReset();
        ds.cacheAlarmsReset();
        break;
    }
    if (ds.isSuccess(ds.setCache()))
    {
      // Display sensor's configuration
      ds.cpyAddress(address);
      ds.cpySernum(sernum);
      Serial.println(String(++deviceNum) + ". Id: " + String(ds.getId()));
      Serial.println("Address: " + String(textAddress(address)));
      Serial.println("Sernum: " + String(textSernum(sernum)));
      Serial.println("Resolution: 0b" + String(ds.getResolution(), BIN) + ", " +
                     String(ds.getResolutionBits()) + " bits" + ", " +
                     String(ds.getResolutionTemp(), 4) + " 'C");
      Serial.println("Conversion: " + String(ds.getConvMillis()) + " ms");
      Serial.println("Alarm Low: " + String(ds.getAlarmLow()) + " 'C");
      Serial.println("Alarm High: " + String(ds.getAlarmHigh()) + " 'C");
      Serial.println("---");
    }
  }
  errorHandler();
  Serial.println("---");
  Serial.println("START_OF_ALARMS");
  Serial.println("---");
}

void loop()
{
  bool flagNoAlarm = true;
  ds.conversion();
  while (ds.isAlarm(ds.alarms()))
  {
    flagNoAlarm = false;
  }
  if (flagNoAlarm)
  {
    Serial.println("No alarms");
    Serial.println("---");
  }
  delay(PERIOD_MEASURE);
}
