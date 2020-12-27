<a id="library"></a>
# gbj_DS18B20
Library for Dallas Semiconductor DS18B20 one-wire temperature sensors.
- Library utilizes a detailed error handling.
- Library does not have conversion to imperial temperature units (degrees of Fahrenheit). The conversion should be provided in a sketch code or in a separate library in order to avoid code duplicities in sketches using multiple libraries with the same conversion functionalities.
- Library provides a _device identifier_ taken from last byte (<abbr title="Cyclic Redundancy Code">CRC</abbr>) of a device's hardware <abbr title="Read Only Memory">ROM</abbr>.
- At temperature alarm processing an alarm temperature is converted from integer to float right before comparison.
- Library is primarily aimed for working with all sensors on the one-wire bus in a loop, so that they need not to be identified by an address in advance. Thus, all getters and setters are valid for currently selected sensor in a loop.

<a id="dependency"></a>
## Dependency

- **OneWire**: Library for communication on one-wire library #1 in PlatformIO Library Manager available at [https://platformio.org/lib/show/1/OneWire](https://platformio.org/lib/show/1/OneWire).

#### Particle platform
- **Particle.h**: Includes alternative (C++) data type definitions.

#### ESP32/ESP8266 platform
- **Arduino.h**: Main include file for the Espressif platform.

#### Arduino platform
- **Arduino.h**: Main include file for the Arduino SDK version greater or equal to 100.
- **WProgram.h**: Main include file for the Arduino SDK version less than 100.
- **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.


<a id="constants"></a>
## Constants

- **gbj\_ds18b20::VERSION**: Name and semantic version of the library.


<a id="params"></a>
#### Parameters for buffers

- **gbj\_ds18b20::ADDRESS\_LEN**  (`gbj_ds18b20::Params::ADDRESS_LEN`): Number of bytes in the sensor's ROM.
- **gbj\_ds18b20::SERNUM\_LEN** (`gbj_ds18b20::Params::SERNUM_LEN`): Number of bytes in the sensor's serial number.
- **gbj\_ds18b20::SCRATCHPAD\_LEN** (`gbj_ds18b20::Params::SCRATCHPAD_LEN`): Number of bytes in the sensor's data buffer.


<a id="results"></a>
#### Result and error codes

- **gbj\_ds18b20::SUCCESS** (`gbj_ds18b20::ResultCodes::SUCCESS`): Successful processing of the recent function.
- **gbj\_ds18b20::END\_OF\_LIST** (`gbj_ds18b20::ResultCodes::END_OF_LIST`): Last sensor on the bus has been processed.
- **gbj\_ds18b20::ERROR\_NO\_DEVICE** (`gbj_ds18b20::ResultCodes::ERROR_NO_DEVICE`): No sensor on the one-wire bus detected.
- **gbj\_ds18b20::ERROR\_CRC\_ADDRESS** (`gbj_ds18b20::ResultCodes::ERROR_CRC_ADDRESS`): Bad hardware address of a sensor.
- **gbj\_ds18b20::ERROR\_CRC\_SCRATCHPAD** (`gbj_ds18b20::ResultCodes::ERROR_CRC_SCRATCHPAD`): Reading of a sensor's data buffer has failed.
- **gbj\_ds18b20::ERROR\_NO\_ALARM** (`gbj_ds18b20::ResultCodes::ERROR_NO_ALARM`): No temperature alarm detected.
- **gbj\_ds18b20::ERROR\_ALARM\_LOW** (`gbj_ds18b20::ResultCodes::ERROR_ALARM_LOW`): Low temperature alarm has been detected.
- **gbj\_ds18b20::ERROR\_ALARM\_HIGH** (`gbj_ds18b20::ResultCodes::ERROR_ALARM_HIGH`): High temperature alarm has been detected.


<a id="interface"></a>
## Interface
It is possible to use functions from the parent library [OneWire](#dependency), which is extended by the [gbj_DS18B20](#library).


##### Custom data types

- [gbj_ds18b20::Address](#address)
- [gbj_ds18b20::Handler()](#handler)
- [gbj_ds18b20::Scratchpad](#scrathpad)
- [gbj_ds18b20::Sernum](#Sernum)


##### Main functions

- [gbj_ds18b20()](#constructor)
- [alarms()](#alarms)
- [conversion()](#conversion)
- [measureTemperature()](#measureTemperature)
- [sensors()](#sensors)


##### Alarm processing

- [getAlarmHigh()](#getAlarm)
- [getAlarmLow()](#getAlarm)
- [isAlarm()](#isAlarm)
- [isAlarmHigh()](#isAlarm)
- [isAlarmLow()](#isAlarm)
- [cacheAlarmHigh()](#cacheAlarm)
- [cacheAlarmLow()](#cacheAlarm)
- [cacheAlarmReset()](#cacheAlarm)
- [setCache()](#setCache)


##### Utilities

- [cpyAddress()](#cpyAddress)
- [cpyScratchpad()](#cpyScratchpad)
- [cpySernum()](#cpySernum)
- [getLastResult()](#getLastResult)
- [isError()](#isResult)
- [isSuccess()](#isResult)
- [setLastResult()](#setLastResult)


<a id="setters"></a>
#### Setters

- [cacheAlarmHigh()](#cacheAlarm)
- [cacheAlarmLow()](#cacheAlarm)
- [cacheAlarmReset()](#cacheAlarm)
- [cacheResolution_10bits()](#cacheResolution)
- [cacheResolution_11bits()](#cacheResolution)
- [cacheResolution_12bits()](#cacheResolution)
- [cacheResolution_9bits()](#cacheResolution)
- [cacheResolutionReset()](#cacheResolution)
- [setCache()](#setCache)
- [setLastResult()](#setLastResult)


<a id="getters"></a>
#### Getters

- [cpyAddress()](#cpyAddress)
- [cpyScratchpad()](#cpyScratchpad)
- [cpySernum()](#cpySernum)
- [getAlarmHigh()](#getAlarm)
- [getAlarmLow()](#getAlarm)
- [getConvMillis()](#getConvMillis)
- [getDevices()](#getDevices)
- [getFamilyCode()](#getFamilyCode)
- [getId()](#getId)
- [getLastResult()](#getLastResult)
- [getPin()](#getPin)
- [getResolution()](#getResolution)
- [getResolutionBits()](#getResolutionBits)
- [getResolutionTemp()](#getResolutionTemp)
- [getSensors()](#getSensors)
- [getTemperature()](#getTemperature)
- [getTemperatureIni()](#getTempLimit)
- [getTemperatureMax()](#getTempLimit)
- [getTemperatureMin()](#getTempLimit)
- [isAlarm()](#isAlarm)
- [isAlarmHigh()](#isAlarm)
- [isAlarmLow()](#isAlarm)
- [isError()](#isResult)
- [isPowerExternal()](#isPower)
- [isPowerParasite()](#isPower)
- [isSuccess()](#isResult)


<a id="address"></a>
## Address

#### Description
Custom data type determining the byte array for sensor hardware address (ROM).
- The size of the address text is determined by the constant [gbj\_ds18b20::ADDRESS\_LEN](#params).

#### Syntax
    gbj_ds18b20::Address address

#### Example
```cpp
gbj_ds18b20::Address address = {0x28, 0xC2, 0x51, 0x6F, 0x06, 0x00, 0x00, 0x6C};
```
#### See also
[Back to interface](#interface)


<a id="handler"></a>
## Handler()

#### Description
Custom data type determining the template for alarm handler procedures.

#### Syntax
    void gbj_ds18b20::Handler()

#### Parameters
None

#### Returns
None

#### Example
```cpp
void alarmHandlerLow()
{
  Serial.println("Low Alarm Handler");
}
gbj_ds18b20 ds = gbj_ds18b20(4, alarmHandlerLow);
```

#### See also
[gbj_ds18b20()](#constructor)

[Back to interface](#interface)


<a id="scratchpad"></a>
## Scratchpad

#### Description
Custom data type determining the byte array for sensor data buffer.
- The size of the address text is determined by the constant [gbj\_ds18b20::SCRATCHPAD\_LEN](#params).

#### Syntax
    gbj_ds18b20::Scratchpad scratchpad

[Back to interface](#interface)


<a id="Sernum"></a>
## Sernum

#### Description
Custom data type determining the byte array for sensor hardware serial number.
- It is an inner part of the sensor ROM without the first family code byte and the last CRC byte.
- The size of the address text is determined by the constant [gbj\_ds18b20::SERNUM\_LEN](#params).
- The sensor's serial number might be considered as a <abbr title="Media Access Control">MAC</abbr> address of it.

#### Syntax
    gbj_ds18b20::Sernum sernum

#### See also
[Address](#Address)

[Back to interface](#interface)


<a id="constructor"></a>
## gbj_ds18b20()

#### Description
Constructor creates the class instance object and alternatively sets alarm handlers.
- Constructor resets the one-wire bus.
- Constructor detects whether some of sensors on the bus is powered in parasitic mode.
- Constructor counts all devices as well as DS18B20 temperature sensors on the bus.
- The results are available by respective getters [getDevices()](#getDevices), [getSensors()](#getSensors).

#### Syntax
    gbj_ds18b20(uint8_t pinBus, gbj_ds18b20::Handler *alarmHandlerLow, gbj_ds18b20::Handler *alarmHandlerHigh)

#### Parameters
<a id="prm_pinBus"></a>
- **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  - *Valid values*: non-negative integer 0 to 255
  - *Default value*: none

<a id="prm_alarmHandlerLow"></a>
- **alarmHandlerLow**: Pointer to a procedure that is called than the measured temperature reaches the low alarms temperature or sinks bellow it.
  - *Valid values*: within address space of the microcontroller by custom type [gbj\_ds18b20::Handler()](#handler).
  - *Default value*: 0 (not used any alarm handler)

<a id="prm_alarmHandlerHigh"></a>
- **alarmHandlerHigh**: Pointer to a procedure that is called than measured temperature reaches the high alarms temperature or is above it.
  - *Valid values*: within address space of the microcontroller by custom type [gbj\_ds18b20::Handler()](#handler).
  - *Default value*: 0 (not used any alarm handler)

#### Returns
Object preforming the temperature measurement.

#### Example
The constructor has the argument for alarm handlers defaulted.

``` cpp
gbj_ds18b20 ds = gbj_ds18b20(4);  // Without alarm handlers
gbj_ds18b20 ds = gbj_ds18b20(4, alarmHandlerLow, alarmHandlerHigh);
```

#### See also
[Handler()](#handler)

[Back to interface](#interface)


<a id="alarms"></a>
## alarms()

#### Description
The method selects DS18B20 sensors active on the one-wire bus one by one, which are in the state of alarm signalling and for each of them reads scratchpad memory for further processing by getters and setters.
- The method [conversion()](#conversion) for parallel temperature measurement should be called right before cycling the alarms.
- The method returns an [error code](#results) for low or high alarm until there is a sensor with active alarm on the bus.
- The type of an alarm or just its instance can be detected by corresponding getter.
- After selecting the last alarm the method resets searching process.

#### Syntax
    gbj_ds18b20::ResultCodes alarms()

#### Parameters
None

#### Returns
Error code about low or high alarm from [Result and error codes](#results) of recently selected sensor with active alarm on the bus.

#### Example
``` cpp
gbj_ds18b20 ds = gbj_ds18b20(4);
void setup()
{
  ds.conversion();
  while (ds.isAlarm(ds.alarms()))
  {
    ...
  }
}
```

#### See also
[conversion()](#conversion)

[isAlarm(), isAlarmLow(), isAlarmHigh()](#isAlarm)

[Back to interface](#interface)


<a id="conversion"></a>
## conversion()

#### Description
The method initiates measurement conversion of all sensors on the one-wire bus at once (parallelly). Then it is possible to read temperature from all sensors without subsequent conversion.

#### Syntax
    gbj_ds18b20::ResultCodes conversion()

#### Parameters
None

#### Returns
Result code from [Result and error codes](#results).

#### See also
[measureTemperature()](#measureTemperature)

[alarms()](#alarms)

[getTemperature()](#getTemperature)

[Back to interface](#interface)


<a id="measureTemperature"></a>
## measureTemperature()

#### Description
The method initiates measurement conversion of the particular sensor with provided address on the one-wire bus and reads its scratchpad right after it.
- The result is available by the getter [getTemperature()](#getTemperature).

#### Syntax
    gbj_ds18b20::ResultCodes measureTemperature(gbj_ds18b20::Address address)

#### Parameters
<a id="prm_address"></a>
- **address**: Array variable with a device ROM identifying a sensor.
  - *Valid values*: array of non-negative integers 0 to 255 with length defined by the constant [gbj\_ds18b20::ADDRESS\_LEN](#params)
  - *Default value*: none

#### Returns
Result code from [Result and error codes](#results).
- If there is no device on the one-wire bus, the method returns the error code [gbj\_ds18b20::ERROR\_NO\_DEVICE](#results).
- If there is a device or more devices on the one-wire bus, but none with input ROM, the method returns the error code [gbj\_ds18b20::ERROR\_CRC\_SCRATCHPAD](#results).

#### See also
[conversion()](#conversion)

[getTemperature()](#getTemperature)

[Back to interface](#interface)


<a id="sensors"></a>
## sensors()

#### Description
The method selects devices with DS18B20 family code and are active on the one-wire bus one by one and for each of them reads scratchpad memory for further processing by getters and setters.
- The method returns success result code until there is an active sensor on the bus.
- After selecting the last sensor the method resets searching process.

#### Syntax
    gbj_ds18b20::ResultCodes sensors()

#### Parameters
None

#### Returns
Result code about selecting recent sensor on the bus defined by one of [Result and error codes](#results).

#### Example
``` cpp
gbj_ds18b20 ds = gbj_ds18b20(4);
void setup()
{
  while (ds.isSuccess(ds.sensors()))
  {
    ...
  }
}
```

#### See also
[isSuccess()](#isResult)

[Back to interface](#interface)


<a id="isAlarm"></a>
## isAlarmLow(), isAlarmHigh(), isAlarm()

#### Description
Appropriate method checks if selected device meets condition for low or high alarm or some of them detected right before.
- Before calling corresponding method the method [alarms()](#alarms) should be called in order to detect alarm conditions. Usually it is called as an input parameter.
- A method without input parameter evaluates result of recent operation, which should be the method [alarms()](#alarms).

#### Syntax
    bool isAlarmLow()
    bool isAlarmLow(gbj_ds18b20::ResultCodes result)

    bool isAlarmHigh()
    bool isAlarmHigh(gbj_ds18b20::ResultCodes result)

    bool isAlarm()
    bool isAlarm(gbj_ds18b20::ResultCodes result)

#### Parameters
<a id="prm_result"></a>
- **result**: Result code of the recent operation detecting alarm conditions.
  - *Valid values*: Some of [gbj\_ds18b20::ERROR\_ALARM\_LOW, gbj\_ds18b20::ERROR\_ALARM\_HIGH](#results).
  - *Default value*: [gbj\_ds18b20::SUCCESS](#results)

#### Returns
Flag about signalling alarm condition.

#### Example
``` cpp
ds.conversion();
while (ds.isAlarm(ds.alarms()))
{
  if (ds.isAlarmLow()) Serial.println("Low alarm");
  if (ds.isAlarmHigh()) Serial.println("High alarm");
}
```
``` cpp
ds.conversion();
while (ds.isAlarmLow(ds.alarms()))
{
  Serial.println("Low alarm");
}
```

#### See also
[alarms()](#alarms)

[conversion()](#conversion)

[Back to interface](#interface)


<a id="getAlarm"></a>
## getAlarmLow(), getAlarmHigh()

#### Description
Appropriate method returns currently set corresponding alarm temperature of selected device in centigrades.
- If alarm signalling capability is not used, alarm condition values can be utilized as a general-purpose memory or 2-byte EEPROM respectively and these methods read stored signed bytes from EEPROM and the scratchpad.

#### Syntax
    int8_t getAlarmLow()
    int8_t getAlarmHigh()

#### Parameters
None

#### Returns
Temperature in centigrades currently set as a low or high alarm condition respectively.

#### See also
[cacheAlarmLow(), cacheAlarmHigh(), cacheAlarmsReset()](#cacheAlarm)

[Back to interface](#interface)


<a id="cacheAlarm"></a>
## cacheAlarmLow(), cacheAlarmHigh(), cacheAlarmsReset()

#### Description
Appropriate method writes alarm condition to internal buffer representing scratchpad.
- Alarm temperatures are in integer centigrades.
- The method _cacheAlarmsReset()_ sets factory default alarm conditions **70 and 75** centigrades.
- If alarm signalling capability is not used, alarm condition values can be utilized as a general-purpose memory or 2-byte EEPROM respectively and these methods write signed bytes to the scratchpad and EEPROM.

#### Syntax
    void cacheAlarmLow(int8_t alarmValue)
    void cacheAlarmHigh(int8_t alarmValue)
    void cacheAlarmsReset()

#### Parameters
- **alarmValue**: Temperature in integer centigrades for appropriate alarm condition.
  - *Valid values*: integer -55 ~ +125 defined by methods [getTemperatureMin(), getTemperatureMax()](#getTemperature)
  - *Default value*: none

#### Returns
None

#### See also
[setCache()](#setCache)

[getAlarmLow(), getAlarmHigh()](#getAlarm)

[alarms()](#alarms)

[Back to interface](#interface)


<a id="cacheResolution"></a>
## cacheResolution_9bits(), cacheResolution_10bits(), cacheResolution_11bits(), cacheResolution_12bits(), cacheResolutionReset()

#### Description
Appropriate method writes temperature measurement resolution in bits from its name to internal buffer representing configuration register of the scratchpad.
- The method _cacheResolutionReset()_ sets factory default resolution 12 bits.

#### Syntax
    void cacheResolution_9bits(void)
    void cacheResolution_10bits(void)
    void cacheResolution_11bits(void)
    void cacheResolution_12bits(void)
    void cacheResolutionReset()

#### Parameters
None

#### Returns
None

#### See also
[setCache()](#setCache)

[getResolution()](#getResolution)

[getResolutionBits()](#getResolutionBits)

[getResolutionTemp()](#getResolutionTemp)

[Back to interface](#interface)


<a id="setCache"></a>
## setCache()

#### Description
The method writes all currently cached parameters to the sensor's scratchpad and EEPROM respectively.

#### Syntax
    gbj_ds18b20::ResultCodes setCache()

#### Parameters
None

#### Returns
Result code from [Result and error codes](#results) about writing scratchpad and EEPROM.

#### See also
[cacheAlarmLow(), cacheAlarmHigh(), cacheAlarmsReset()](#cacheAlarm)

[cacheResolution_9bits(), cacheResolution_10bits(), cacheResolution_11bits(), cacheResolution_12bits(), cacheResolutionReset()](#cacheResolution)

[Back to interface](#interface)


<a id="getResolution"></a>
## getResolution()

#### Description
The method returns current resolution as a binary value from the configuration register in the scratchpad according to the datasheet.
- The resolution determines the temperature conversion time needed at least.

#### Syntax
    uint8_t getResolution()

#### Parameters
None

#### Returns
Temperature resolution as an internal code from configuration register (R1, R0).

#### See also
[getResolutionBits()](#getResolutionBits)

[getResolutionTemp()](#getResolutionTemp)

[getConvMillis()](#getConvMillis)

[Back to interface](#interface)


<a id="getResolutionBits"></a>
## getResolutionBits()

#### Description
The method returns current resolution expressed in bits.

#### Syntax
    uint8_t getResolutionBits()

#### Parameters
None

#### Returns
Number of bits used for temperature measurement resolution in range 9 ~ 12.

#### See also
[getResolution()](#getResolution)

[getResolutionTemp()](#getResolutionTemp)

[Back to interface](#interface)


<a id="getResolutionTemp"></a>
## getResolutionTemp()

#### Description
The method returns current resolution of the selected device expressed in centigrades.

#### Syntax
    float getResolutionTemp()

#### Parameters
None

#### Returns
Temperature resolution of measurement in the range 0.0625 ~ 0.5 centigrade.

#### See also
[getResolution()](#getResolution)

[getResolutionBits()](#getResolutionBits)

[Back to interface](#interface)


<a id="getTemperature"></a>
## getTemperature()

#### Description
The method returns recently measured temperature.
- It is useful for repeating utilizing the temperature without storing it in a separate variable in a sketch.

#### Syntax
    float getTemperature()

#### Parameters
None

#### Returns
Recently measured temperature.

#### See also
[conversion()](#conversion)

[measureTemperature()](#measureTemperature)

[Back to interface](#interface)


<a id="getTempLimit"></a>
## getTemperatureMin(), getTemperatureMax(), getTemperatureIni()

#### Description
Particular method returns corresponding typical temperature limit or value for the DS18B20 sensors.
- Methods _getTemperatureMin()_, _getTemperatureMax()_ return temperature measurement limits -55 ~ +125 centigrade.
- Method _getTemperatureIni()_ returns power-on reset temperature 85 centigrade.

#### Syntax
    float getTemperatureMin()
    float getTemperatureMax()
    float getTemperatureIni()

#### Parameters
None

#### Returns
Typical temperature measurement limit and value.

#### See also
[getResolutionTemp()](#getResolutionTemp)

[Back to interface](#interface)


<a id="getFamilyCode"></a>
## getFamilyCode()

#### Description
The method returns product identifier as the very first byte of the sensors ROM.

#### Syntax
    uint8_t getFamilyCode()

#### Parameters
None

#### Returns
Device specific internal code for product identification of the sensor. For DS18B20 it is always HEX 28, DEC 40.

[Back to interface](#interface)


<a id="getId"></a>
## getId()

#### Description
The method returns identifier of the selected device as the CRC code of the device ROM from its last byte.

#### Syntax
    uint8_t getId()

#### Parameters
None

#### Returns
Short device identifier unique for all devices on the one-wire bus.

#### See also
[getFamilyCode()](#getFamilyCode)

[Back to interface](#interface)


<a id="getPin"></a>
## getPin()

#### Description
The method returns number of microcontroller pin mastering the one-wire bus.
- It is the [pinBus](#prm_pinBus) parameter of the [constructor](#constructor).

#### Syntax
    uint8_t getPin()

#### Parameters
None

#### Returns
Microcontroller pin number controlling one-wire bus.

#### See also
[gbj_ds18b20()](#constructor)

[Back to interface](#interface)


<a id="getConvMillis"></a>
## getConvMillis()

#### Description
The method returns number of milliseconds needed for temperature conversion depending on current temperature resolution.
- The return value defines a waiting period with respect to the datasheet that should last before subsequent new temperature measurement.
- The conversion time determines maximal measurement frequency.

#### Syntax
    uint8_t getConvMillis()

#### Parameters
None

#### Returns
Number of milliseconds that the temperature conversion is going to last at least.

#### See also
[getResolution()](#getResolution)

[Back to interface](#interface)


<a id="getDevices"></a>
## getDevices()

#### Description
The method returns number of active (communicating) devices on the one-wire bus of all families (not only DS18B20 sensors).

#### Syntax
    uint8_t getDevices()

#### Parameters
None

#### Returns
Number of devices on the one-wire bus.

#### See also
[getSensors()](#getSensors)

[Back to interface](#interface)


<a id="getSensors"></a>
## getSensors()

#### Description
The method returns number of active (communicating) DS18B20 temperature sensors on the one-wire bus.

#### Syntax
    uint8_t getSensors()

#### Parameters
None

#### Returns
Number of DS18B20 sensors on the one-wire bus.

#### See also
[getDevices()](#getDevices)

[Back to interface](#interface)


<a id="isPower"></a>
## isPowerExternal(), isPowerParasite()

#### Description
The appropriate method determines whether a device is in correponding mode of supplying.

#### Syntax
    bool isPowerExternal()
    bool isPowerParasite()

#### Parameters
None

#### Returns
Flag about external or parasitic power mode.

[Back to interface](#interface)


<a id="setLastResult"></a>
## setLastResult()

#### Description
The method sets or initializes the internal status of recent processing on the one-wire bus to input value.
- Without input parameter the method initializes internal status to success result code with class constant [gbj\_ds18b20::SUCCESS](#results).
- The method without input parameter is usually called right before any operation on the bus in order to reset the internal status or in methods without bus communication.

#### Syntax
    gbj_ds18b20::ResultCodes setLastResult()
    gbj_ds18b20::ResultCodes setLastResult(gbj_ds18b20::ResultCodes result)

#### Parameters
<a id="prm_result"></a>
- **result**: Desired result code that should be set as a recent result code.
  - *Valid values*: Some of [Result or error codes](#results).
  - *Default value*: [gbj\_ds18b20::SUCCESS](#results)

#### Returns
New (actual) result code of recent operation from [Result and error codes](#results).

#### See also
[getLastResult()](#getLastResult)

[Back to interface](#interface)


<a id="getLastResult"></a>
## getLastResult()

#### Description
The method returns a result code of the recent operation on the one-wire bus. It is usually called for error handling in a sketch.

#### Syntax
    gbj_ds18b20::ResultCodes getLastResult()

#### Parameters
None

#### Returns
Result code of the recent processing from [Result and error codes](#results).

#### See also
[setLastResult()](#setLastResult)

[Back to interface](#interface)


<a id="isSuccess"></a>
## isSuccess(), isError()

#### Description
The method returns a logical flag whether the recent operation on the one-wire bus was successful or failed respectivelly.
- The corresponding result code can be obtained by the method [getLastResult()]((#getLastResult).

#### Syntax
    bool isSuccess()
    bool isError()

#### Parameters
None

#### Returns
Flag about successful or failed processing of the recent operation on the bus.

#### See also
[getLastResult()](#getLastResult)

[Back to interface](#interface)


<a id="cpyAddress"></a>
## cpyAddress()

#### Description
The method copies the ROM into a provided input byte array.

#### Syntax
    void cpyAddress(gbj_ds18b20::Address address)

#### Parameters
- **address**: Array variable for receiving device ROM.

#### Returns
Populated input array with device ROM.

#### Example
```cpp
gbj_ds18b20::Address address;
void setup()
{
  while (ds.isSuccess(ds.sensors()))
  {
    ds.cpyAddress(address);
  }
}
```

#### See also
[Address](#address)

[Back to interface](#interface)


<a id="cpyScratchpad"></a>
## cpyScratchpad()

#### Description
The method copies the current cache of the scratchpad into a provided input byte array.

#### Syntax
    void cpyScratchpad(gbj_ds18b20::Scratchpad scratchpad)

#### Parameters
- **scratchpad**: Array variable for receiving device scratchpad.

#### Returns
Populated input array with current device scratchpad cache content.

#### Example
```cpp
gbj_ds18b20::Scratchpad scratchpad;
void setup()
{
  while (ds.isSuccess(ds.sensors()))
  {
    ds.cpyScratchpad(scratchpad);
  }
}
```

#### See also
[Scratchpad](#scratchpad)

[Back to interface](#interface)


<a id="cpySernum"></a>
## cpySernum()

#### Description
The method copies the current cache of the scratchpad into a provided input byte array.

#### Syntax
    void cpySernum(gbj_ds18b20::Sernum sernum)

#### Parameters
- **sernum**: Array variable for receiving device serial number as a part of ROM.

#### Returns
Populated input array with device serial number.

#### Example
```cpp
gbj_ds18b20::Sernum sernum;
void setup()
{
  while (ds.isSuccess(ds.sensors()))
  {
    ds.cpySernum(sernum);
  }
}
```

#### See also
[Sernum](#Sernum)

[Back to interface](#interface)
