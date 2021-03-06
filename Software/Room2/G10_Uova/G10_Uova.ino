//Teensy 3.2

#include <MPR121.h>

//#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  12       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  6       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {0,0,0,0,0,0,0,0,0,0,0,0}; // mani uova
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {1, 2, 3, 5, 6, 7}; // luci uova

//04:E9:E5:07:A5:DF
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x07, 0xA5, 0xDF}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 110};                     //This needs to be unique in your network - only one puzzle can have this IP

bool started = false;
int players = 0;

void resetSpec() {
  started = false;
}

#include <EscapeFunction.h>

bool handOnEggs[SENNUM] = {false, false, false, false, false, false, false, false, false, false, false, false};
bool eggCaptured[SENNUM / 2] = {false, false, false, false, false, false};

void setup() {
  setupEscape();
  // specifico per questo gioco
  // 0x5A is the MPR121 I2C address on the Bare Touch Board
  if (!MPR121.begin(0x5A)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    CPU_RESTART;
  } else Serial.println("MPR121 ok");

  // this is the touch threshold - setting it low makes it more like a proximity trigger
  // default value is 40 for touch
  MPR121.setTouchThreshold(120);

  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121.setReleaseThreshold(80);

  // initial data update
  MPR121.updateTouchData();

  players = 6; //Mb.R[60];
}

void loop() {
  Mb.Run();
  listenFromEth();
//
//  // Su comando da Control Room si attiva la sequenza luci
//  if (gameActivated && !started) {
//    //flashing();
//    started = true;
//  }

  if (!triggered && gameActivated) {
    gameUpdate();
    isPuzzleSolved();
  }
  //printRegister();
}

void gameUpdate() {
  MPR121.updateTouchData();
  for (int i = 0; i < 2*players; i++) {
    if (MPR121.isNewTouch(i)) {
      sensorRegUpdate(i, 1);
      handOnEggs[i] = true;
      Serial.println("Hand " + (String)(i) + "touched");
      delay(10);
    } else if (MPR121.isNewRelease(i)) {
      sensorRegUpdate(i, 0);
      handOnEggs[i] = false;
    }
  }
//  for (int i = 0; i < 2*players; i = i + 2) {
//    if ((handOnEggs[i] + handOnEggs[i + 1]) > 1) {
//      digitalWrite(devPins[i / 2], HIGH);
//      deviceRegUpdate(i/2, HIGH);
//      eggCaptured[i / 2] = true;
//      Serial.println("Egg " + (String)(i / 2) + "touched");
//    }
//    else {
//      digitalWrite(devPins[i / 2], LOW);
//      deviceRegUpdate(i/2, LOW);
//      eggCaptured[i / 2] = false;
//      //Serial.println("Egg " + (String)(i/2) + "untouched");
//    }
//    delay(10);
//  }
//  puzzleSolved = isAllEggsTouched();
}

bool isAllEggsTouched() {
  for ( int i = 0; i < players; ++i ) {
    if ( eggCaptured[ i ] == false ) {
      return false;
    }
  }
  return true;
}

void flashing() {
  uint8_t h = 0;
  Serial.println("Flashing");
  while (h < 10) {
    for (uint8_t b = 0; b < players; b++) {
      digitalWrite(devPins[b], HIGH);
      Mb.R[DEVICES[b]] = HIGH;
      myDelay(300);
      digitalWrite(devPins[b], LOW);
      Mb.R[DEVICES[b]] = LOW;
    }
    h++;
  }
}
