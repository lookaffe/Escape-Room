//Teensy LC

#define ONLINE

#define SENNUM  3       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {20, 19, 18}; // u1, u2, u3
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {};

//04:E9:E5:06:66:F6
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0x66, 0xF6}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 111};                     //This needs to be unique in your network - only one puzzle can have this IP

bool pressed[SENNUM] = {0, 0, 0};

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
  printRegister();
}

void gameUpdate() {
  bool ps = true;
  for(int i = 0; i<SENNUM; i++) {
    bool pres = digitalRead(senPins[i]);
    ps = ps && pres;
    sensorRegUpdate(i, pres);
    Serial.print(" Uccello " + (String)i + ": pres - " + (String)pres);
  }
  Serial.println();
  puzzleSolved = ps;
  stateRegUpdate(puzzleSolved);
}
