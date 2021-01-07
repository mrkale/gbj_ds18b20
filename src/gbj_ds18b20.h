/*
  NAME:
  gbj_ds18b20

  DESCRIPTION:
  Library for temperature sensors Dallas Semiconductor DS18B20.
  - Library does not have conversion to imperial temperature units (degrees
    of Fahrenheit), which should be provided in a sketch code or in a separate
    library in order to avoid code duplicities in sketches using multiple
    libraries with the same conversion functionalities.
  - Library provides a device identifier taken from last (CRC) byte of a
    device's hardware ROM address.
  - At temperature alarm processing an alarm temperature is converted from integer
    to float right before comparison.
  - Library is primarily aimed for working with all sensors on the one-wire bus
    in a loop, so that they need not to be identified by an address in advance.
    Thus, all getters and setters are valid for currently selected sensor
    in a loop.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/gbj_ds18b20.git
 */
#ifndef GBJ_DS18B20_H
#define GBJ_DS18B20_H

#if defined(__AVR__)
  #include <Arduino.h>
  #include <inttypes.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <Arduino.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif
#include <OneWire.h>

class gbj_ds18b20 : public OneWire
{
public:
  static const String VERSION;

  enum ResultCodes : uint8_t
  {
    SUCCESS = 0,
    END_OF_LIST = 1,
    ERROR_NO_DEVICE = 255,
    ERROR_CRC_ADDRESS = 254,
    ERROR_CRC_SCRATCHPAD = 253,
    ERROR_NO_ALARM = 252,
    ERROR_ALARM_LOW = 251,
    ERROR_ALARM_HIGH = 250,
    ERROR_CONVERSION = 249,
  };

  enum Params : uint8_t
  {
    FAMILY_CODE = 0x28,
    ADDRESS_LEN = 8,
    SERNUM_LEN = 6,
    SCRATCHPAD_LEN = 9,
  };

  typedef uint8_t Address[Params::ADDRESS_LEN];
  typedef uint8_t Sernum[Params::SERNUM_LEN];
  typedef uint8_t Scratchpad[Params::SCRATCHPAD_LEN];
  typedef void Handler();

  /*
    Constructor

    DESCRIPTION:
    Constructor creates the class instance object and alternatively sets
    alarm handlers.

    PARAMETERS:
    pinBus - Number of GPIO pin of the microcontroller managing one-wire bus.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255

    alarmHandlerLow - Pointer to a procedure that is called than the measured
      temperature reaches the low alarms temperature.
      - Data type: handler
      - Default value: 0
      - Limited range: system address range

    alarmHandlerHigh - Pointer to a procedure that is called than
      measured temperature reaches the high alarms temperature.
      - Data type: handler
      - Default value: 0
      - Limited range: system address range

    RETURN: object
  */
  gbj_ds18b20(uint8_t pinBus,
              Handler *alarmHandlerLow = 0,
              Handler *alarmHandlerHigh = 0)
    : OneWire(pinBus)
  {
    _bus.pinBus = pinBus;
    _bus.alarmHandlerLow = alarmHandlerLow;
    _bus.alarmHandlerHigh = alarmHandlerHigh;
    if (isError(powering()))
      return;
    devices();
  }

  /*
    Iterate over supported sensors on the bus

    DESCRIPTION:
    The method selects devices with supported family code and active
    on the one-wire bus one by one and for each of them reads scratchpad
    memory for further processing by getters and setters.

    PARAMETERS: None

    RETURN: Result code.
  */
  ResultCodes sensors();

  /*
    Iterate over supported sensors on the bus with alarm signalling

    DESCRIPTION:
    The method selects devices with supported family code and active
    on the one-wire bus one by one, which are in the state of alarm signaling
    and for each of them reads scratchpad memory for further processing
    by getters and setters.

    PARAMETERS: None

    RETURN: Result code.
  */
  ResultCodes alarms();

  /*
    Execute bulk temperature conversion.

    DESCRIPTION:
    The method initiates measurement conversion of all sensors on the
    one-wire bus at once (parallelly). Then it is possible to read temperature
    from all sensors without subsequent conversion.

    PARAMETERS: none

    RETURN: Result code.
  */
  ResultCodes conversion();

  /*
    Execute temperature measurement by individual sensor.

    DESCRIPTION:
    The method initiates measurement conversion of the particular sensor
    with provided address on the one-wire bus and reads its scratchpad right
    after it and returns temperature.

    PARAMETERS:
    address - Temperature sensor address.
      - Data type: array of non-negative integers
      - Default value: none
      - Limited range: 0 ~ 255[8]

    RETURN: Result code.
  */
  ResultCodes measureTemperature(const Address address);

  // Public setters
  inline ResultCodes setLastResult(ResultCodes result = ResultCodes::SUCCESS)
  {
    return _status.lastResult = result;
  }
  inline void cacheResolution_9bits() { _memory.scratchpad.config = 0x1F; }
  inline void cacheResolution_10bits() { _memory.scratchpad.config = 0x3F; }
  inline void cacheResolution_11bits() { _memory.scratchpad.config = 0x5F; }
  inline void cacheResolution_12bits() { _memory.scratchpad.config = 0x7F; }
  inline void cacheResolutionReset() { cacheResolution_12bits(); }
  inline void cacheAlarmLow(int8_t alarmValue)
  {
    _memory.scratchpad.alarm_lsb =
      constrain(alarmValue, (int)getTemperatureMin(), (int)getTemperatureMax());
  }
  inline void cacheAlarmHigh(int8_t alarmValue)
  {
    _memory.scratchpad.alarm_msb =
      constrain(alarmValue, (int)getTemperatureMin(), (int)getTemperatureMax());
  }
  inline void cacheAlarmsReset()
  {
    cacheAlarmLow(getAlarmLowIni());
    cacheAlarmHigh(getAlarmHighIni());
  }
  inline ResultCodes setCache() { return writeScratchpad(); }

  // Public getters
  inline ResultCodes getLastResult() { return _status.lastResult; }
  inline ResultCodes getCache() { return readScratchpad(); }
  inline bool isSuccess() { return _status.lastResult == ResultCodes::SUCCESS; }
  inline bool isSuccess(ResultCodes result)
  {
    setLastResult(result);
    return isSuccess();
  }
  inline bool isError() { return !isSuccess(); }
  inline bool isError(ResultCodes result)
  {
    setLastResult(result);
    return isError();
  }
  inline bool isAlarmLow()
  {
    return _status.lastResult == ResultCodes::ERROR_ALARM_LOW;
  }
  inline bool isAlarmLow(ResultCodes result)
  {
    setLastResult(result);
    return isAlarmLow();
  }
  inline bool isAlarmHigh()
  {
    return _status.lastResult == ResultCodes::ERROR_ALARM_HIGH;
  }
  inline bool isAlarmHigh(ResultCodes result)
  {
    setLastResult(result);
    return isAlarmHigh();
  }
  inline bool isAlarm() { return isAlarmLow() || isAlarmHigh(); }
  inline bool isAlarm(ResultCodes result)
  {
    setLastResult(result);
    return isAlarmLow() || isAlarmHigh();
  }
  inline int8_t getAlarmLow() { return _memory.scratchpad.alarm_lsb; }
  inline int8_t getAlarmHigh() { return _memory.scratchpad.alarm_msb; }
  inline int8_t getAlarmLowIni() { return 70; }
  inline int8_t getAlarmHighIni() { return 75; }
  inline uint8_t getPin() { return _bus.pinBus; }
  inline uint8_t getDevices() { return _bus.devices; }
  inline uint8_t getSensors() { return _bus.sensors; }
  inline uint8_t getFamilyCode() { return _rom.address.family; }
  inline uint8_t getId() { return _rom.address.crc; }
  inline float getTemperatureMin() { return -55.0; }
  inline float getTemperatureMax() { return 125.0; }
  inline float getTemperatureIni() { return 85.0; }
  inline bool isPowerExternal() { return _bus.powerExternal; }
  inline bool isPowerParasite() { return !isPowerExternal(); }
  inline uint8_t *getAddressRef() { return _rom.buffer; }
  inline uint8_t *getScratchpadRef() { return _memory.buffer; }
  inline void cpyAddress(Address address)
  {
    memcpy(address, _rom.buffer, Params::ADDRESS_LEN);
  }
  inline void cpySernum(Sernum sernum)
  {
    memcpy(sernum, _rom.address.sernum, Params::SERNUM_LEN);
  }
  inline void cpyScratchpad(Scratchpad scratchpad)
  {
    memcpy(scratchpad, _memory.buffer, Params::SCRATCHPAD_LEN);
  }
  inline uint8_t getResolutionBits() { return _bus.tempBits[getResolution()]; }
  inline float getResolutionTemp() { return _bus.tempDegs[getResolution()]; }
  inline uint8_t getResolution()
  {
    uint8_t resolution = _memory.scratchpad.config >> ConfigRegBit::R0;
    return resolution & 0b11;
  }
  float getTemperature()
  {
    int16_t temp = _memory.scratchpad.temp_msb << 8;
    temp |= _memory.scratchpad.temp_lsb & _bus.tempMask[getResolution()];
    return (float)temp / 16.0;
  }
  inline uint8_t getConvMillis() { return _bus.tempMillis[getResolution()]; }

private:
  enum ConfigRegBit : uint8_t
  {
    R0 = 5,
    R1 = 6,
  };

  enum CommandsRom : uint8_t
  {
    SEARCH_ROM = 0xF0,
    READ_ROM = 0x33,
    MATCH_ROM = 0x55,
    SKIP_ROM = 0xCC,
    ALARM_SEARCH = 0xEC,
  };

  enum CommandsFnc : uint8_t
  {
    CONVERT_T = 0x44,
    WRITE_SCRATCHPAD = 0x4E,
    READ_SCRATCHPAD = 0xBE,
    COPY_SCRATCHPAD = 0x48,
    RECALL = 0xB8,
    READ_POWER_SUPPLY = 0xB4,
  };

  union ROM
  {
    uint8_t buffer[Params::ADDRESS_LEN];
    struct Address
    {
      uint8_t family;
      uint8_t sernum[6];
      uint8_t crc;
    } address;
  } _rom;

  union Memory
  {
    uint8_t buffer[Params::SCRATCHPAD_LEN];
    struct Scratchpad
    {
      uint8_t temp_lsb;
      uint8_t temp_msb;
      uint8_t alarm_msb;
      uint8_t alarm_lsb;
      uint8_t config;
      uint8_t res_ff;
      uint8_t res_0c;
      uint8_t res_10;
      uint8_t crc;
    } scratchpad;
  } _memory;

  struct Bus
  {
    // Resolutions in bits - Values { 0x1F, 0x3F, 0x5F, 0x7F }
    uint8_t tempBits[4] = { 9, 10, 11, 12 };
    uint8_t tempMask[4] = { 0xF8, 0xFC, 0xFE, 0xFF }; // LSB bits masks
    uint16_t tempMillis[4] = {
      94,
      188,
      375,
      750
    }; // Maximal conversion times in milliseconds
    float tempDegs[4] = { 0.5,
                          0.25,
                          0.125,
                          0.0625 }; // Resolutions in centigrades
    uint8_t pinBus;
    uint8_t resolution; // The highest resolution of all devices
    bool powerExternal; // Flag about all devices powered externally
    uint8_t devices; // The number of all devices on the bus
    uint8_t sensors; // The number of temperature sensors on the bus
    Handler *alarmHandlerLow; // Global alarm handlers
    Handler *alarmHandlerHigh;
  } _bus;

  struct Status
  {
    ResultCodes lastResult;
  } _status;

  ResultCodes devices(); // Statistics of devices on the bus
  ResultCodes powering(); // Detect power mode
  ResultCodes cpyRom(const Address address); // Copy address to ROM buffer
  inline void resetRom() { memset(_rom.buffer, 0, Params::ADDRESS_LEN); }
  ResultCodes conversionWait();
  ResultCodes readScratchpad();
  ResultCodes writeScratchpad();
  inline void resetScratchpad()
  {
    memset(_memory.buffer, 0, Params::SCRATCHPAD_LEN);
  }
};

#endif
