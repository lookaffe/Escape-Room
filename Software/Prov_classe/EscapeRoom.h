/*
  EscapeRoom.h - Library for Escape Room Modbus based using Mudbus library.
  Created by Luca Feliciani, November 29, 2018.
  Released into the public domain.
*/

#ifndef EscapeRoom_h
#define EscapeRoom_h

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <vector>

#define STATE 0
#define RESET 100
#define ACTIVE 124

class EscapeRoom
{
  public:
    EscapeRoom(uint8_t senNum, uint8_t actNum, uint8_t devNum, bool alwaysActive);
    void gameUpdate();
    void isPuzzleSolved();
    void triggerAct(int actPin, boolean trig);
    void resetAll();
    void listenFromEth();
    void printRegister();
  private:
    uint8_t _senNum, _actNum, _devNum;
    bool _puzzleSolved = false, _triggered = false, _gameActivated;
    std::vector<uint8_t> SENSORS, ACTUATORS, DEVICES;
};

#endif
