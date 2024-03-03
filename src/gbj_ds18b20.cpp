#include "gbj_ds18b20.h"

gbj_ds18b20::ResultCodes gbj_ds18b20::powering()
{
  setLastResult();
  reset();
  skip();
  write(CommandsFnc::READ_POWER_SUPPLY);
  bus_.powerExternal = read_bit();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::devices()
{
  setLastResult();
  // Count all active devices on the bus
  bus_.devices = 0;
  bus_.sensors = 0;
  while (search(rom_.buffer))
  {
    if (rom_.address.crc != crc8(rom_.buffer, Params::ADDRESS_LEN - 1))
    {
      return setLastResult(ResultCodes::ERROR_CRC_ADDRESS);
    }
    bus_.devices++;
    // Count all active temperature sensors on the bus
    if (getFamilyCode() == Params::FAMILY_CODE)
    {
      bus_.sensors++;
      // Detect maximal resolution of all active temperature sensors on the bus
      if (isSuccess(readScratchpad()))
      {
        bus_.resolution = max(bus_.resolution, getResolution());
      }
    }
  }
  reset_search();
  if (bus_.devices == 0)
  {
    setLastResult(ResultCodes::ERROR_NO_DEVICE);
  }
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::sensors()
{
  static uint8_t iterations;
  setLastResult();
  while (search(rom_.buffer))
  {
    if (getFamilyCode() != Params::FAMILY_CODE)
    {
      continue;
    }
    if (isSuccess(readScratchpad()))
    {
      iterations++;
    }
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
  bus_.sensors = iterations;
  iterations = 0;
  reset_search();
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::alarms()
{
  static uint8_t iterations;
  // Conditional search
  while (search(rom_.buffer, false))
  {
    if (getFamilyCode() != Params::FAMILY_CODE)
    {
      continue;
    }
    if (isSuccess(readScratchpad()))
    {
      iterations++;
      // Alarm low
      if (getTemperature() <= getAlarmLow())
      {
        if (bus_.alarmHandlerLow)
        {
          bus_.alarmHandlerLow();
        }
        setLastResult(ResultCodes::ERROR_ALARM_LOW);
      }
      // Alarm high
      if (getTemperature() >= getAlarmHigh())
      {
        if (bus_.alarmHandlerHigh)
        {
          bus_.alarmHandlerHigh();
        }
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
  select(rom_.buffer);
  write(CommandsFnc::READ_SCRATCHPAD);
  read_bytes(memory_.buffer, Params::SCRATCHPAD_LEN);
  // Check zero config register - no sensor on the bus
  if (memory_.scratchpad.config == 0)
  {
    return setLastResult(ResultCodes::ERROR_NO_DEVICE);
  }
  // Check scratchpad CRC
  if (memory_.scratchpad.crc !=
      crc8(memory_.buffer, Params::SCRATCHPAD_LEN - 1))
  {
    return setLastResult(ResultCodes::ERROR_CRC_SCRATCHPAD);
  }
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::writeScratchpad()
{
  setLastResult();
  reset();
  select(rom_.buffer);
  write(CommandsFnc::WRITE_SCRATCHPAD);
  write(memory_.scratchpad.alarm_msb, isPowerParasite());
  write(memory_.scratchpad.alarm_lsb, isPowerParasite());
  write(memory_.scratchpad.config, isPowerParasite());
  // Read scratchpad for checking
  if (isError(readScratchpad()))
  {
    return getLastResult();
  }
  // Copy scratchpad to EEPROM
  select(rom_.buffer);
  write(CommandsFnc::COPY_SCRATCHPAD);
  // Wait 10 ms in parasitic power mode according to datasheet
  if (isPowerParasite())
  {
    delay(10);
  }
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::conversion()
{
  setLastResult();
  reset();
  skip();
  write(CommandsFnc::CONVERT_T, isPowerParasite());
  return conversionWait();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::measureTemperature(const Address address)
{
  if (isError(cpyRom(address)))
  {
    return getLastResult();
  }
  reset();
  select(rom_.buffer);
  write(CommandsFnc::CONVERT_T, isPowerParasite());
  if (isSuccess(conversionWait()))
  {
    readScratchpad();
  }
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::cpyRom(const Address address)
{
  setLastResult();
  resetRom();
  // Check ROM CRC
  if (address[Params::ADDRESS_LEN - 1] !=
      crc8(address, Params::ADDRESS_LEN - 1))
  {
    return setLastResult(ResultCodes::ERROR_CRC_ADDRESS);
  }
  // Copy address to buffer
  memcpy(rom_.buffer, address, Params::ADDRESS_LEN);
  return getLastResult();
}

gbj_ds18b20::ResultCodes gbj_ds18b20::conversionWait()
{
  setLastResult();
  if (bus_.powerExternal)
  {
    // Read time slot
    uint32_t tsConv = millis();
    while (!read_bit())
    {
      if (millis() - tsConv > getConvMillis())
      {
        setLastResult(ResultCodes::ERROR_CONVERSION);
        break;
      }
      continue;
    }
  }
  else
  {
    delay(getConvMillis()); // Waiting conversion time period
  }
  return getLastResult();
}
