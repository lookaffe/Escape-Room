//Teensy LC

#define ONLINE

#define SENNUM  1       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  1       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

//Stati
#define FIORE 0     // stato in attesa della presenza de FIORE
#define TOTEM 1     // stato in attesa della soluzione del TOTEM

const int senPins[SENNUM] = {20}; // pulsante fiore
const int actPins[ACTNUM] = {21}; // relayFiore
const int devPins[DEVNUM] = {0};
//04:E9:E5:05:B6:FC
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x05, 0xB6, 0xFC}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 105};                     //This needs to be unique in your network - only one puzzle can have this IP

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
      actuatorRegUpdate(0, LOW);
      fiore = !digitalRead(senPins[0]);
      sensorRegUpdate(0, fiore);
      if (fiore) {
        myDelay(1000);
        actuatorRegUpdate(0, fiore); // attiva l'elettrocalamita
      }
      stato = fiore;
      break;

    case TOTEM:
      ec = deviceRegRead(0); // se lorenzo può cambiarmi lo stato del registro...
      if(ec) actuatorRegUpdate(0, !ec); //disattiva l'elettrocalamita
      puzzleSolved = ec;
      break;
  }
}
