#include "gbj_ds18b20.h"
const String gbj_ds18b20::VERSION = "GBJ_DS18B20 1.0.0";

gbj_ds18b20::ResultCodes gbj_ds18b20::powering()
{
  setLastResult();
  reset();
  skip();
  write(CommandsFnc::READ_POWER_SUPPLY);
  _bus.powerExternal = read_bit();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::devices()
{
  setLastResult();
  // Count all devices on the bus
  _bus.devices = 0;
  _bus.sensors = 0;
  while (search(_rom.buffer))
  {
    if (_rom.address.crc != crc8(_rom.buffer, Params::ADDRESS_LEN - 1))
      return setLastResult(ResultCodes::ERROR_CRC_ADDRESS);
    _bus.devices++;
    if (getFamilyCode() == Limits::DS18B20)
    {
      _bus.sensors++;
      if (isSuccess(readScratchpad()))
        _bus.resolution = max(_bus.resolution, getResolution());
    }
  }
  reset_search();
  if (_bus.devices == 0)
    setLastResult(ResultCodes::ERROR_NO_DEVICE);
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::sensors()
{
  static uint8_t iterations;
  while (search(_rom.buffer))
  {
    if (getFamilyCode() != Limits::DS18B20)
      continue;
    if (isSuccess(readScratchpad()))
      iterations++;
    return getLastResult();
  }
  if (iterations)
  {
    setLastResult(ResultCodes::END_OF_LIST);
  }
  else
  {
    setLastResult(ResultCodes::ERROR_NO_DEVICE);
  }
  iterations = 0;
  reset_search();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::alarms()
{
  static uint8_t iterations;
  // Conditional search
  while (search(_rom.buffer, false))
  {
    if (getFamilyCode() != Limits::DS18B20)
      continue;
    if (isSuccess(readScratchpad()))
    {
      iterations++;
      // Alarm low
      if (getTemperature() <= getAlarmLow())
      {
        if (_bus.alarmHandlerLow)
          _bus.alarmHandlerLow();
        setLastResult(ResultCodes::ERROR_ALARM_LOW);
      }
      // Alarm high
      if (getTemperature() >= getAlarmHigh())
      {
        if (_bus.alarmHandlerHigh)
          _bus.alarmHandlerHigh();
        setLastResult(ResultCodes::ERROR_ALARM_HIGH);
      }
    }
    return getLastResult();
  }
  if (iterations)
  {
    setLastResult(ResultCodes::END_OF_LIST);
  }
  else
  {
    setLastResult(ResultCodes::ERROR_NO_ALARM);
  }
  iterations = 0;
  reset_search();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::readScratchpad()
{
  setLastResult();
  resetScratchpad();
  reset();
  select(_rom.buffer);
  write(CommandsFnc::READ_SCRATCHPAD);
  read_bytes(_memory.buffer, Params::SCRATCHPAD_LEN);
  // Check scratchpad CRC
  if (_memory.scratchpad.crc !=
      crc8(_memory.buffer, Params::SCRATCHPAD_LEN - 1))
    return setLastResult(ResultCodes::ERROR_CRC_SCRATCHPAD);
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::writeScratchpad()
{
  setLastResult();
  reset();
  select(_rom.buffer);
  write(CommandsFnc::WRITE_SCRATCHPAD);
  write(_memory.scratchpad.alarm_msb, isPowerParasite());
  write(_memory.scratchpad.alarm_lsb, isPowerParasite());
  write(_memory.scratchpad.config, isPowerParasite());
  // Read scratchpad for checking
  if (isError(readScratchpad()))
    return getLastResult();
  // Copy scratchpad to EEPROM
  select(_rom.buffer);
  write(CommandsFnc::COPY_SCRATCHPAD);
  // Wait 10 ms in parasitic power mode according to datasheet
  if (isPowerParasite())
    delay(10);
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::conversion()
{
  setLastResult();
  reset();
  skip();
  write(CommandsFnc::CONVERT_T, isPowerParasite());
  conversionWait();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::measureTemperature(const Address address)
{
  if (isError(cpyRom(address)))
    return getLastResult();
  reset();
  select(_rom.buffer);
  write(CommandsFnc::CONVERT_T, isPowerParasite());
  conversionWait();
  return readScratchpad();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::cpyRom(const Address address)
{
  setLastResult();
  resetRom();
  // Check ROM CRC
  if (address[Params::ADDRESS_LEN - 1] !=
      crc8(address, Params::ADDRESS_LEN - 1))
    return setLastResult(ResultCodes::ERROR_CRC_ADDRESS);
  // Copy address to buffer
  memcpy(_rom.buffer, address, Params::ADDRESS_LEN);
  return getLastResult();
}