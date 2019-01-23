#define SENNUM  4       //total amount of sensors
#define ACTNUM  4       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {21, 20, 19, 18}; // acqua, pulsanti, fiaccola, ingranaggio
const int actPins[ACTNUM] = {0, 1, 2}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x99}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};                     //This needs to be unique in your network - only one puzzle can have this IP

#include <EscapeFunction.h>

const int           led                 = 13;
bool                ledState            = false;
unsigned long       timer;

void setup() {
  setupEscape();
  pinMode(led, OUTPUT);

  // blink a few times
  for (int i = 0; i < 5; i++) {
    digitalWrite(led, HIGH);
    myDelay(300);
    digitalWrite(led, LOW);
    delay(300);
  }

  // COP is not serviced, the Teensy LC will reset within 1 sec
}


void loop() {

  timer = millis() + 10000UL;
  while (true) {
    ledState = !ledState;
    digitalWrite(led, ledState);
    myDelay(1000);
    if (millis() < timer) {
      listenFromEth();
    }
  } // while
}

//void myDelay(unsigned long d) {
//  elapsedMillis t;
//  while (t < d) {
//    Watchdog.reset();
//  }
//}


