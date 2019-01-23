//Teensy 3.2

/* se necessario il reset di valori specifici per il gioco decommentare le due linee sottostanti
    scrivere la relativa funzione resetSpec
*/
//#define SPECRESET
//#include "resetSpec.h"

// Used device 0 for Teensy, 1 for ARDUINO
#define DEVICE 0 //

#define SENNUM  4       //total amount of sensors
#define ACTNUM  4       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

//Stati
#define TASTO 0     // stato in attesa della pressione del TASTO
#define ACQUA 1     // stato in attesa della pressione dell'ACQUA
#define FIACCOLA 2  // stato in attesa della pressione della FIACCOLA
#define CIAMBELLA 3 // stato in attesa della pressione della CIAMBELLA

#define SMOKEINTERVAL 5000  // intervallo tra una fumata e l'altra
#define WATERLEVEL 50      // valore per presenza acqua

const int senPins[SENNUM] = {20, 21, 19, 18}; // pulsanti, acqua, fiaccola, ingranaggio
const int actPins[ACTNUM] = {0, 1, 2}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};                     //This needs to be unique in your network - only one puzzle can have this IP

#include <Bounce.h>
Bounce buttonsStart = Bounce(senPins[0], 100);
Bounce fiaccola = Bounce(senPins[2], 100);
Bounce ingranaggio = Bounce(senPins[3], 100);

#include <Servo.h>
//

long smokeStartTime, smokeTime;
bool orologio = true;
int water = 0;

uint8_t stato = TASTO;

void resetSpec() {
  stato = 0;
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
  bool pressed = 0;
  switch (stato) {
    case TASTO:
      Serial.print("STATo "); Serial.println(stato);
      buttonsStart.update();
      pressed = buttonsStart.fallingEdge();
      sensorRegUpdate(stato, pressed);
      if (pressed) stato = ACQUA;
      break;

    case ACQUA:
      Serial.print("STATo "); Serial.println(stato);
      smokeTime = millis();
      if (orologio) {
        //tutti i giri da fare con il motore per l'orologio
        myDelay(3000);
        orologio = false;
        smokeStartTime, smokeTime = millis();
      }
      if (smokeTime - smokeStartTime > SMOKEINTERVAL) {
        smoke();
        smokeStartTime = millis();
      }
      water = analogRead(senPins[1]);
      sensorRegUpdate(stato, water);
      Serial.print("Acqua "); Serial.println(water);
      if (water > WATERLEVEL) stato = FIACCOLA;
      break;

    case FIACCOLA:
      Serial.print("STATo "); Serial.println(stato);
      fiaccola.update();
      pressed = fiaccola.fallingEdge();
      Serial.print("pressed "); Serial.println(pressed);
      sensorRegUpdate(stato, pressed);
      if (pressed) {
        digitalWrite(actPins[1], LOW); // apri sportello ingranaggi
        actuatorRegUpdate(1, HIGH);
        stato = CIAMBELLA;
      }
      break;

    case CIAMBELLA:
      Serial.print("STATo "); Serial.println(stato);
      ingranaggio.update();
      pressed = ingranaggio.fallingEdge();
      Serial.print("pressed "); Serial.println(pressed);
      sensorRegUpdate(stato, pressed);
      if (pressed) {
        digitalWrite(actPins[2], LOW); // attiva il motore degli ingranaggi
        // fai muovere l'orologio
        puzzleSolved = 1;
        stato = 5;
      }
      break;
  }
  //  sensorRegUpdate(0, puzzleSolved);
  //  sensorRegUpdate(1, digitalRead(senPins[1]));
}

void smoke() {
  Serial.println("FUMO!");
  digitalWrite(actPins[0], LOW);
  actuatorRegUpdate(0, HIGH);
  myDelay(1000);
  digitalWrite(actPins[0], HIGH);
  actuatorRegUpdate(0, LOW);
}



