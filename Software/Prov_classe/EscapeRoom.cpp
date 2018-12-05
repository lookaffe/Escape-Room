#include "Arduino.h"
#include "EscapeRoom.h"

EscapeRoom::EscapeRoom(uint8_t senNum, uint8_t actNum, uint8_t devNum, bool alwaysActive)
{
  _senNum = senNum;
  _actNum = actNum;
  _devNum = devNum;
  _gameActivated = alwaysActive;

  //Modbus Registers Offsets (0-9999)
  _SENSORS.resize(_senNum);
  for (uint8_t i = 0; i < _senNum; i++) {
    _SENSORS[i] = i + 1;
  }
  _ACTUATORS.resize(_actNum);
  for (uint8_t i = 0; i < _actNum; i++) {
    _ACTUATORS[i] = i + 1;
  }
  _DEVICES.resize(_devNum);
  for (uint8_t i = 0; i < _devNum; i++) {
    _DEVICES[i] = i + 1;
  }

  //ModbusIP object
  Mudbus Mb;
}



