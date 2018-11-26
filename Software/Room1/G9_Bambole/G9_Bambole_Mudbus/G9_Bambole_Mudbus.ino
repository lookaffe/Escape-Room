//Teensy 3.2 

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Wire.h>
#include <MPR121.h>

#define SENNUM  8 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  8 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x02, 0xB9, 0x25}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 109};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8};
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58};
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins

const int actPins[ACTNUM] = {}; // relay
const int devPins[DEVNUM] = { 0, 1, 2, 3, 4, 5, 6, 7} ;

int sequence[DEVNUM] = {0, 1, 1, 1, 0, 0, 0, 0};      //the right sequence
int yourSequence[DEVNUM] = {0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

boolean sensStatus[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0};

//ModbusIP object
Mudbus Mb;

void setup() {
  // reset for Ethernet Shield
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  delay(1000);
  digitalWrite(9, HIGH); // release the WIZ820io

  Ethernet.begin(mac, ip);
  delay(5000);

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = false;
  Mb.R[ACTIVE] = gameActivated;

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
    while (1);
  } else Serial.println("Tutto ok");

  // this is the touch threshold - setting it low makes it more like a proximity trigger
  // default value is 40 for touch
  MPR121.setTouchThreshold(120);

  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121.setReleaseThreshold(80);

  // initial data update
  MPR121.updateTouchData();

  for (int i = 0; i < DEVNUM; i++) {
    pinMode(devPins[i], OUTPUT);
    digitalWrite(devPins[i], LOW);
  }
}

void loop() {
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  MPR121.updateTouchData();
  for (int i = 0; i < SENNUM; i++) {
    if (MPR121.isNewTouch(i)) {
      Mb.R[SENSORS[i]] = 1;
      Mb.R[DEVICES[i]] = 1;
      digitalWrite(devPins[i], HIGH);
      yourSequence[i] = 1;
    } else if (MPR121.isNewRelease(i)) {
      Mb.R[SENSORS[i]] = 0;
      Mb.R[DEVICES[i]] = 0;
      digitalWrite(devPins[i], LOW);
      yourSequence[i] = 0;
    }
  }
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < DEVNUM; n++) if (a[n] != b[n]) return false;
  return true;
}

void isPuzzleSolved() {
  puzzleSolved = (seq_cmp(yourSequence, sequence)) ? true : false;
  triggered = puzzleSolved;
  Mb.R[STATE] = puzzleSolved;
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  digitalWrite(actPins[s], !trig);
  delay(10);
}

// Resetta il gioco
void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(i, LOW);
  }
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], LOW);
    Mb.R[DEVICES[i]] = LOW;
  }
  triggered = false;
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;
  if (!ALWAYSACTIVE) {
    gameActivated = false;
    Mb.R[ACTIVE] = gameActivated;
  }

  // reset specifico per il gioco
  for (int i = 0; i < SENNUM ; i++) {
    yourSequence[i] = LOW;
  }
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  else {
    triggered = Mb.R[STATE];
    for (int i = 0; i < SENNUM ; i++) {
      sensStatus[i] = Mb.R[SENSORS[i]];
    }
    for (int i = 0; i < ACTNUM ; i++) {
      trigger(i, Mb.R[ACTUATORS[i]]);
      triggered = triggered || Mb.R[ACTUATORS[i]];
    }
    for (int i = 0; i < DEVNUM ; i++) {
      digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
    }
    puzzleSolved = Mb.R[STATE];
    if (Mb.R[STATE]) {
      for (int i = 0; i < ACTNUM ; i++) {
        trigger(i, Mb.R[STATE]);
      }
    }
    gameActivated = Mb.R[ACTIVE];
  }
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  for (int i = 0; i < DEVNUM; i++) {
    Serial.print("DEVICES "); Serial.print(i); Serial.print(" (reg "); Serial.print(DEVICES[i]); Serial.print(") - val:  "); Serial.println(Mb.R[DEVICES[i]]);
  }
  Serial.print("ACTIVATION: "); Serial.println(Mb.R[ACTIVE]);
  Serial.println();
}

