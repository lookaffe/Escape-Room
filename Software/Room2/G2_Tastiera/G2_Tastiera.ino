//Teensy LC

//#define ONLINE

#define SENNUM  4       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {20, 19, 18, 17}; // m1, m2, m3, m4
const int actPins[ACTNUM] = {21}; // relayLuce
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0x66, 0xF4}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 102};                     //This needs to be unique in your network - only one puzzle can have this IP

bool pressed[SENNUM] = {0, 0, 0, 0};

void resetSpec() {
  for (int i = 0; i < SENNUM; i++) pressed[i] = 0;
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
  //printRegister();
}

void gameUpdate() {
  puzzleSolved = !digitalRead(senPins[0]);
  for(int i = 0; i<SENNUM; i++) {
    bool pres = !digitalRead(senPins[i]);
    puzzleSolved = puzzleSolved && pres;
    sensorRegUpdate(i, pres);
  }
  stateRegUpdate(puzzleSolved);
  actuatorRegUpdate(0, puzzleSolved);
}
