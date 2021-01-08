/*
  NAME:
  Unit tests of library "gbj_DS18B20" with connected real device.

  DESCRIPTION:
  The test suite provides test cases for all relevant public methods.
  - The test runner is Unity Project - ThrowTheSwitch.org.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include <Arduino.h>
#include <unity.h>
#include <gbj_ds18b20.h>

// Basic setup
const unsigned char PIN_ONEWIRE = 4; // Pin for one-wire bus
gbj_ds18b20 ds = gbj_ds18b20(PIN_ONEWIRE);
gbj_ds18b20::Address address = {
  0x28, 0xC2, 0x51, 0x6F, 0x06, 0x00, 0x00, 0x6C // Replace with current one
};
// Expected ambient temperature in centigrades and alarms
float tempRoom = 25.0;
float tempDelta = 5.0;
int8_t alarmLow = -15;
int8_t alarmHigh = 25;

void test_setup_familycode(void)
{
  TEST_ASSERT_EQUAL_HEX8(0x28, address[0]);
}

void test_setup_crc(void)
{
  TEST_ASSERT_EQUAL_HEX8(address[ds.ADDRESS_LEN - 1],
                         ds.crc8(address, ds.ADDRESS_LEN - 1));
}

void test_bus_sensors(void)
{
  TEST_ASSERT_TRUE(ds.getSensors() >= 1);
}

void test_device_measure(void)
{
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(ds.SUCCESS,
                                  ds.measureTemperature(address),
                                  "Probably specified sensor not on the bus!");
}

void test_device_familycode(void)
{
  TEST_ASSERT_EQUAL_UINT8(address[0], ds.getFamilyCode());
}

void test_device_id(void)
{
  TEST_ASSERT_EQUAL_UINT8(address[ds.ADDRESS_LEN - 1], ds.getId());
}

void test_device_pin(void)
{
  TEST_ASSERT_EQUAL_UINT8(PIN_ONEWIRE, ds.getPin());
}

void test_device_temp_ini(void)
{
  TEST_ASSERT_FALSE(ds.getTemperature() == ds.getTemperatureIni());
}

void test_device_temp_min(void)
{
  TEST_ASSERT_TRUE_MESSAGE(ds.getTemperature() >= ds.getTemperatureMin(),
                           "Power-up condition.");
}

void test_device_temp_max(void)
{
  TEST_ASSERT_TRUE(ds.getTemperature() <= ds.getTemperatureMax());
}

void test_device_temp_cur(void)
{
  TEST_ASSERT_FLOAT_WITHIN(tempDelta, tempRoom, ds.getTemperature());
}

void test_device_cache_set(void)
{
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
    ds.SUCCESS, ds.setCache(), "Problem with writing to the scratchpad!");
}

void test_device_cache_get(void)
{
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
    ds.SUCCESS, ds.getCache(), "Problem with reading the scratchpad!");
}

void test_device_alarm_low(void)
{
  ds.cacheAlarmLow(alarmLow);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_INT8(alarmLow, ds.getAlarmLow());
}

void test_device_alarm_high(void)
{
  ds.cacheAlarmHigh(alarmHigh);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_INT8(alarmHigh, ds.getAlarmHigh());
}

void test_device_alarm_low_factory(void)
{
  ds.cacheAlarmsReset();
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_INT8(ds.getAlarmLowIni(), ds.getAlarmLow());
}

void test_device_alarm_high_factory(void)
{
  ds.cacheAlarmsReset();
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_INT8(ds.getAlarmHighIni(), ds.getAlarmHigh());
}

void test_device_resolution_9(void)
{
  ds.cacheResolutionBits(9);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_UINT8(9, ds.getResolutionBits());
}

void test_device_resolution_10(void)
{
  ds.cacheResolutionBits(10);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_UINT8(10, ds.getResolutionBits());
}

void test_device_resolution_11(void)
{
  ds.cacheResolutionBits(11);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_UINT8(11, ds.getResolutionBits());
}

void test_device_resolution_12(void)
{
  ds.cacheResolutionBits(12);
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_UINT8(12, ds.getResolutionBits());
}

void test_device_resolution_factory(void)
{
  ds.cacheResolutionReset();
  test_device_cache_set();
  test_device_cache_get();
  TEST_ASSERT_EQUAL_UINT8(12, ds.getResolutionBits());
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_setup_familycode);
    RUN_TEST(test_setup_crc);

    RUN_TEST(test_bus_sensors);

    RUN_TEST(test_device_measure);
    RUN_TEST(test_device_familycode);
    RUN_TEST(test_device_id);
    RUN_TEST(test_device_pin);

    RUN_TEST(test_device_temp_ini);
    RUN_TEST(test_device_temp_min);
    RUN_TEST(test_device_temp_max);
    RUN_TEST(test_device_temp_cur);

    RUN_TEST(test_device_alarm_low);
    RUN_TEST(test_device_alarm_high);
    RUN_TEST(test_device_alarm_low_factory);
    RUN_TEST(test_device_alarm_high_factory);

    RUN_TEST(test_device_resolution_12);
    RUN_TEST(test_device_resolution_11);
    RUN_TEST(test_device_resolution_10);
    RUN_TEST(test_device_resolution_9);
    RUN_TEST(test_device_resolution_factory);

    UNITY_END();
}

void loop(){}