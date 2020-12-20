/*
  NAME:
  Temperature measurement by a single DS18B20 sensor with provided address.

  DESCRIPTION:
  The sketch uses definite sensor for measurement identified by provided address.
  - The sketch provides the scratchpad memory dump for better observing sensors.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "gbj_ds18b20.h"

#define SKETCH "GBJ_DS18B20_SINGLE 1.0.0"

const unsigned char PIN_DS18B20 = 4; // Pin for one-wire bus

gbj_ds18b20 ds = gbj_ds18b20(PIN_DS18B20);
gbj_ds18b20::Address address = {
  0x28, 0xBB, 0xA0, 0x6E, 0x06, 0x00, 0x00, 0x86
};
gbj_ds18b20::Serial serial;
gbj_ds18b20::Scratchpad scratchpad;
char buffer[50];

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

String textSerial(gbj_ds18b20::Serial serial)
{
  String text = "";
  char data[3];
  for (byte i = 0; i < gbj_ds18b20::SERIAL_LEN; i++)
  {
    if (i)
      text += ".";
    sprintf(data, "%02X", serial[i]);
    text += data;
  }
  return text;
}

String textScratchpad(gbj_ds18b20::Scratchpad scratchpad)
{
  String text = "";
  char data[3];
  for (byte i = 0; i < gbj_ds18b20::SCRATCHPAD_LEN; i++)
  {
    if (i)
      text += ":";
    sprintf(data, "%02X", scratchpad[i]);
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

    case gbj_ds18b20::ERROR_CRC_ADDRESS:
      Serial.println("ERROR_CRC_ADDRESS");
      break;

    case gbj_ds18b20::ERROR_CRC_SCRATCHPAD:
      Serial.println("ERROR_CRC_SCRATCHPAD");
      break;

    case gbj_ds18b20::ERROR_NO_DEVICE:
      Serial.println("ERROR_NO_DEVICE");
      break;

    default:
      Serial.println("Unknown error");
      break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println(); // Some serial monitors display garbage at the begining
  Serial.println(SKETCH);
  Serial.println("Libraries:");
  Serial.println(gbj_ds18b20::VERSION);
  Serial.println("---");
  Serial.println("Address: " + String(textAddress(address)));
  if (ds.isSuccess(ds.measureTemperature(address)))
  {
    ds.cpySerial(serial);
    Serial.println("Id: " + String(ds.getId(), HEX));
    Serial.println("Serial: " + String(textSerial(serial)));
    Serial.println("Resolution: 0b" + String(ds.getResolution(), BIN) + ", " +
                   String(ds.getResolutionBits()) + " bits" + ", " +
                   String(ds.getResolutionTemp(), 4) + " 'C");
    Serial.println("Temperature: " + String(ds.getTemperature(), 4) + " 'C");
    Serial.println("Alarm Low: " + String(ds.getAlarmLow(), 1) + " 'C");
    Serial.println("Alarm High: " + String(ds.getAlarmHigh(), 1) + " 'C");
  }
  ds.cpyScratchpad(scratchpad);
  Serial.println("Scratchpad: " + String(textScratchpad(scratchpad)));
  Serial.println("---");
  errorHandler();
}

void loop() {}
