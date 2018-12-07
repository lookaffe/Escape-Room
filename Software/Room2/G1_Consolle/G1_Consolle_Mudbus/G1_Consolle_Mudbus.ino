//Teensy LC

/* se necessario il reset di valori specifici per il gioco decommentare le due linee sottostanti
    scrivere la relativa funzione resetSpec
*/
//#define SPECRESET
//#include "resetSpec.h"

#define SENNUM  2 //total amount of sensors
#define ACTNUM  1 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices
#define ALWAYSACTIVE 1 //1 if the game is always active

const int senPins[SENNUM] = {16, 22}; // switch, door
const int actPins[ACTNUM] = {21}; // relay
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x99}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 101};     //181                  //This needs to be unique in your network - only one puzzle can have this IP

#include <EscapeFunction.h>

#include <Bounce.h>
Bounce button0 = Bounce(senPins[0], 100); //Pin button

void setup()
{
  setupEscape();
}

void loop()
{
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  button0.update();

  if (button0.risingEdge()) puzzleSolved = true;
  sensorRegUpdate(0, puzzleSolved);
  sensorRegUpdate(1, digitalRead(senPins[1]));
}



