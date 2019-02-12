//Teensy LC

//#define ONLINE

#define SENNUM  1       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  1       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

//Stati
#define FIORE 0     // stato in attesa della presenza de FIORE
#define TOTEM 1     // stato in attesa della soluzione del TOTEM

const int senPins[SENNUM] = {21}; // pulsante fiore
const int actPins[ACTNUM] = {5}; // relayFiore
const int devPins[DEVNUM] = {14};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 104};                     //This needs to be unique in your network - only one puzzle can have this IP

uint8_t stato = FIORE;

void resetSpec() {
  stato = FIORE;
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
  bool fiore = 0, ec = 0;
  switch (stato) {
    case FIORE: // in attesa del fiore, elettrocalamita scollegata
      actuatorRegUpdate(0, 1);
      fiore = !digitalRead(senPins[0]);
      sensorRegUpdate(0, fiore);
      if (fiore) {
        actuatorRegUpdate(0, !fiore); // attiva l'elettrocalamita
        deviceRegUpdate(0, fiore); // usato per comunicare a Lorenzo l'attivazione del play sul totem
      }
      stato = fiore;
      break;

    case TOTEM:
      ec = deviceRegRead(0); // se lorenzo può cambiarmi lo stato del registro...
      if(!ec) actuatorRegUpdate(0, ec); //disattiva l'elettrocalamita
      puzzleSolved = !ec;
      break;
  }
}
