//Teensy 3.2

/* se necessario il reset di valori specifici per il gioco decommentare le due linee sottostanti
    scrivere la relativa funzione resetSpec
*/
//#define SPECRESET
//#include "resetSpec.h"

#define SENNUM  4 //total amount of sensors
#define ACTNUM  4 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices
#define ALWAYSACTIVE 1 //1 if the game is always active

//Stati
#define START 0
#define TASTO 1
#define ACQUA 2
#define FIACCOLA 3
#define CIAMBELLA 4

const int senPins[SENNUM] = {21, 20, 19, 18}; // acqua, pulsanti, fiaccola, ingranaggio
const int actPins[ACTNUM] = {22, 0, 1, 2}; // servo, relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x99}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};                     //This needs to be unique in your network - only one puzzle can have this IP

#include <EscapeFunction.h>

#include <Bounce.h>
Bounce button0 = Bounce(senPins[1], 100);

#include <Servo.h>
Servo watch;

uint8_t stato = START;
void setup()
{
  //setupEscape();
  watch.attach(actPins[0]);
  watch.write(0);
  delay (2000);
  for (int pos = 0; pos <= 270; pos += 1) {
        watch.write(pos);              // tell servo to go to position in variable 'pos'
        delay(50);                       // waits 15ms for the servo to reach the position
      Serial.print("pos "); Serial.println(pos);
      }
}

void loop()
{
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }

  //printRegister();
}

void gameUpdate() {
  switch (stato) {
    case START:
      Serial.print("STAT0 "); Serial.println(stato);
      button0.update();
      if (button0.fallingEdge()) stato = TASTO;
      break;
    case TASTO:
      Serial.print("STAT0 "); Serial.println(stato);
      for (int pos = 0; pos <= 180; pos += 1) {
        watch.write(pos);              // tell servo to go to position in variable 'pos'
        delay(50);                       // waits 15ms for the servo to reach the position
      }
      break;
    case ACQUA:
      //
      break;
    case FIACCOLA:
      //
      break;
    case CIAMBELLA:
      //
      break;
  }
  //
  //
  //  sensorRegUpdate(0, puzzleSolved);
  //  sensorRegUpdate(1, digitalRead(senPins[1]));
}



