//Teensy 3.2

#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  3       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {20, 21, 19, 18}; // pulsanti, acqua, fiaccola, ingranaggio
const int actPins[ACTNUM] = {0, 1, 2}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};                     //This needs to be unique in your network - only one puzzle can have this IP

void resetSpec() {
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
}




