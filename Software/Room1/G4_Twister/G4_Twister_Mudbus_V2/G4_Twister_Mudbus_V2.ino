//Teensy 3.2

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Wire.h>
#include <MPR121a.h>
#include "Adafruit_MPR121.h"

#define SENNUM  24 //total amount of sensors
#define ACTNUM  2 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

#define HANDS 12
#define SAMPLE 50

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 110};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
const int ACTUATORS[ACTNUM] = {101, 102};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int actPins[ACTNUM] = {22, 23}; // relay
const int devPins[DEVNUM] = {} ;

int sequence[SENNUM] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0};    //the right sequence
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //user sequence

boolean sensStatus[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//ModbusIP object
Mudbus Mb;

Adafruit_MPR121 cap_hands = Adafruit_MPR121();

uint16_t dataMatrix[SAMPLE][HANDS];
float capValue[HANDS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int thres = 65;
uint8_t row = 0;

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

  Serial.println("Adafruit MPR121 cap_hands active Touch sensor test");
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap_hands.begin(0x5B)) {
    Serial.println("MPR121 1 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 1 found!");

  // specifico per questo gioco
  pinMode(actPins[0], OUTPUT);
  pinMode(actPins[1], OUTPUT);
  digitalWrite(actPins[0], HIGH);
  digitalWrite(actPins[1], HIGH);

  // 0x5A is the MPR121 I2C address on the Bare Touch Board
  if (!MPR121a.begin(0x5A)) {
    Serial.println("error setting up MPR121");
    switch (MPR121a.getError()) {
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
  MPR121a.setTouchThreshold(40);

  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121a.setReleaseThreshold(20);

  // initial data update
  MPR121a.updateTouchData();

  for (int i = 0; i < HANDS; i++) {
    for (int j = 0; j < SAMPLE; j++) {
      dataMatrix[i][j] = 0;
    }
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
  //BareConductive
  MPR121a.updateTouchData();
  for (int i = HANDS; i < SENNUM; i++) {
    if (MPR121a.isNewTouch(i - HANDS)) {
      Mb.R[SENSORS[i]] = 1;
      yourSequence[i] = 1;
      //printFeetValues(i, yourSequence[i]);     // Debug for feet
    } else if (MPR121a.isNewRelease(i - HANDS)) {
      Mb.R[SENSORS[i]] = 0;
      yourSequence[i] = 0;
      //printFeetValues(i, yourSequence[i]);     // Debug for feet
    }
  }

  //Adafruit
  if (row == SAMPLE) {
    checkTouch();
    //printHandsValues();    //Debug for hands
    row = 0;
  }
  for (int col = 0; col < HANDS; col++) {
    dataMatrix[row][col] = cap_hands.filteredData(col);
  }
  row++;
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < SENNUM; n++) if (a[n] != b[n]) return false;
  return true;
}

void checkTouch() {
  for (int i = 0; i < HANDS; i++) {
    float capAVG = 0;
    for (int j = 0; j < SAMPLE; j++) {
      capAVG = capAVG + dataMatrix[j][i];
    }
    capValue[i] = capAVG / SAMPLE;
    if ( capValue[i] < thres) {
      Mb.R[SENSORS[i]] = 1;
      yourSequence[i] = 1;
    } else {
      Mb.R[SENSORS[i]] = 0;
      yourSequence[i] = 0;
    }
  }
}

void printHandsValues() {
  Serial.print("Filt: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS; i++) {
    Serial.print(yourSequence[i]); Serial.print("\t");
  }
  Serial.println();

  Serial.print("Push: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS; i++) {
    Serial.print(capValue[i]); Serial.print("\t");
  }
  Serial.println();
}

void printFeetValues(int i, bool a) {
  Serial.print("electrode ");
  Serial.print(i + 1, DEC);
  a ? Serial.println(" was just touched") :  Serial.println(" was just released");
}

void isPuzzleSolved() {
  puzzleSolved = (seq_cmp(yourSequence, sequence)) ? true : false;
  triggered = puzzleSolved;
  trigger(0, puzzleSolved);
  trigger(1, puzzleSolved);
  Mb.R[STATE] = puzzleSolved;
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  Serial.print(" was just touched");Serial.println(trig);
  digitalWrite(actPins[s], !trig);
  delay(10);
}

// Resetta il gioco
void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(i, LOW);
  }
  triggered = false;
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], LOW);
    Mb.R[DEVICES[i]] = LOW;
  }
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
    for (int i = 0; i < SENNUM ; i++) {
      sensStatus[i] = Mb.R[SENSORS[i]];
    }
    triggered = 0;
    for (int i = 0; i < ACTNUM ; i++) {
      trigger(i, Mb.R[ACTUATORS[i]]);
      triggered = triggered || Mb.R[ACTUATORS[i]];
    }
    for (int i = 0; i < DEVNUM ; i++) {
      digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
    }
    puzzleSolved = Mb.R[STATE];
    if (Mb.R[STATE]) triggered = Mb.R[STATE];
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
