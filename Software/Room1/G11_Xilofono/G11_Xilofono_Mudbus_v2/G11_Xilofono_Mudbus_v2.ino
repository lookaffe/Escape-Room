
// TEENSY 3.5
/*
   Pin Tasto PLAY 19
   Pin Tasto RESET 35
   Pin Luce PLAY 21
   Pin Luce RESET 33
   Pin Pulsanti Elettrovalvole 17, 15, 22, 20, 18, 16
   Pin Pulsanti Martelletti 25, 24, 26, 28, 30, 32
   Pin Relay Elettrovalvole 0, 2, 4, 6, 8, 7
   Pin Servo 23
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  14 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM 9 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

// per questo gioco------
#define VALNUM 6
#define HAMNUM 6
// ----------------------

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE4}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 111};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; // 1 playButton, 2:7 valvButton, 8 resetButton, 9:14 hamButton
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58, 59}; // 52 playLight, 54:60 valvRelay 53 resetLight, 51 servo
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {19, 17, 15, 22, 20, 18, 16, 35, 25, 24, 26, 28, 30, 32}; // // 19 playButton, 17:16 valvButton, 35 resetButton, 25:32 hammButton
const int actPins[ACTNUM] = {}; // relay
const int devPins[DEVNUM] = {21, 0, 2, 4, 6, 8, 7, 33, 23} ; // , 21 playLight, 0:7 valvRelay, 33 resetLight, 23 servo

int sensStatus[SENNUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int lightPin[SENNUM] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;
int sequence[SENNUM] = {0, 1, 1, 0, 0, 0 , 1, 1, 1, 0};      //the right sequence
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

Bounce button0 = Bounce(sensPins[0], 100); //Pin button

// per questo gioco------
const int playLightPin = 21;
const int playButtonPin = 19;
boolean playState = false;

const int resetLightPin = 33;
const int resetButtonPin = 35;
boolean resetState = false;

const int valvPins[VALNUM] = {0, 2, 4, 6, 8, 7};
const int valvButtonPins[VALNUM] = {17, 15, 22, 20, 18, 16};
int valvButtonState[VALNUM] = {0, 0, 0, 0, 0, 0};

const int hammButtonPins[HAMNUM] = {25, 24, 26, 28, 30, 32};
int hammButtonState[HAMNUM] = {0, 0, 0, 0, 0, 0};

const long fluxTime = 200; // 2900 per 0,5 cl

int waterSteps[VALNUM] = {8, 6, 5, 10, 3, 1};  //quantità d'acqua per le note giuste Verde, Blu, Rosa, Rosso, Giallo, Nero
int yourWaterSteps[VALNUM] = {0, 0, 0, 0, 0, 0};
int lastSteps[VALNUM] = {0, 0, 0, 0, 0, 0}; //only for debugging

int sequence[HAMNUM] = {0, 1, 1, 0, 0, 0};      //the right sequence
int yourSequence[HAMNUM] = {0, 0, 0, 0, 0, 0};   //user sequence

unsigned long interrupt_time = 0;

Bounce button0 = Bounce(hammButtonPins[0], 10);
Bounce button1 = Bounce(hammButtonPins[1], 10);  // 10 = 10 ms debounce time
Bounce button2 = Bounce(hammButtonPins[2], 10);  // which is appropriate for
Bounce button3 = Bounce(hammButtonPins[3], 10);  // most mechanical pushbuttons
Bounce button4 = Bounce(hammButtonPins[4], 10);
Bounce button5 = Bounce(hammButtonPins[5], 10);

//-----------------------

//ModbusIP object
Mudbus Mb;

void setup() {
  Serial.begin(9600);
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

  //Set Pin mode
  for (int i = 0; i < VALNUM; i++) {
    pinMode(valvPins[i], OUTPUT);
    pinMode(valvButtonPins[i], INPUT);
    digitalWrite(valvPins[i], LOW);  // LOW se uso SSRelay
  }
  for (int i = 0; i < HAMNUM; i++) {
    pinMode(hammButtonPins[i], INPUT);
  }
  pinMode(playButtonPin, INPUT);
  pinMode(playLightPin, OUTPUT);
  digitalWrite(playLightPin, LOW);

  pinMode(resetButtonPin, INPUT);
  pinMode(resetLightPin, OUTPUT);
  digitalWrite(resetLightPin, HIGH);

  interrupt_time = millis();
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printSteps();
  //printRegister();
}

void gameUpdate() {
  playState = seq_cmp(yourWaterSteps, waterSteps, VALNUM);
  digitalWrite(resetLightPin, !playState);
  digitalWrite(playLightPin, playState);
  if (!playState) fillTheGlasses();
  else playInstrument();
}

void fillTheGlasses() {
  resetState = !digitalRead(resetButtonPin);
  if (resetState) {
    for (int i = 0; i < VALNUM; i++) {
      yourWaterSteps[i] = 0;
    }
    emptiesGlasses();
  }
  for (int i = 0; i < VALNUM; i++) {
    valvButtonState[i] = digitalRead(valvButtonPins[i]);
    Mb.R[SENSORS[i + 1]] = !valvButtonState[i];
    if (!valvButtonState[i]) {
      openValv(i);
      yourWaterSteps[i] = yourWaterSteps[i] + 1;
    }
  }
}

void openValv(int v) {
  digitalWrite(valvPins[v], HIGH); // HIGH se uso SSRelay
  delay(fluxTime);
  digitalWrite(valvPins[v], LOW);
}

void emptiesGlasses() {
  // servo
}

void playInstrument() {
  buttonUpdate();
  if (millis() - interrupt_time > 3000) {
    interrupt_time = millis();
    if (seq_cmp(yourSequence, sequence)) puzzleSolved = true;
    else {
      puzzleSolved = false;
      seq_clear(yourSequence);
    }
    Mb.R[STATE] = puzzleSolved;
  }
  isPuzzleSolved();
}

void buttonUpdate() {
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();

  if (button0.fallingEdge()) {
    fallingEdgeAction(0);
  }
  if (button1.fallingEdge()) {
    fallingEdgeAction(1);
  }
  if (button2.fallingEdge()) {
    fallingEdgeAction(2);
  }
  if (button3.fallingEdge()) {
    fallingEdgeAction(3);
  }
  if (button4.fallingEdge()) {
    fallingEdgeAction(4);
  }
  if (button5.fallingEdge()) {
    fallingEdgeAction(5);
  }
}

void fallingEdgeAction(int b) {
  hammButtonState[b] = hammButtonState[b] + 1;
  yourSequence[b] = hammButtonState[b];
  Mb.R[SENSORS[b]] = hammButtonState[b][b];
  interrupt_time = millis();
}

//compare the two sequences
boolean seq_cmp(int *a, int *b, int siz) {
  for (int n = 0; n < siz; n++) if (a[n] != b[n]) return false;
  return true;
}

//clear the sequence
void seq_clear(int *s) {
  for (int n = 0; n < SENNUM; n++)  {
    s[n] = 0;
    sensStatus[n] = LOW;
    Mb.R[SENSORS[n]] = sensStatus[n];
    pressed = 0;
    interrupt_time = millis();
  }
}

void isPuzzleSolved() {
  Mb.R[STATE] = puzzleSolved;
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  digitalWrite(s, trig);
  delay(10);
}

// Resetta il gioco
void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], LOW);
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
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  else {
    for (int i = 0; i < SENNUM ; i++) {
      sensStatus[i] = Mb.R[SENSORS[i]];
    }
    triggered = 0;
    for (int i = 0; i < ACTNUM ; i++) {
      trigger(actPins[i], Mb.R[ACTUATORS[i]]);
      triggered = triggered || Mb.R[ACTUATORS[i]];
    }
    for (int i = 0; i < DEVNUM ; i++) {
      switch (i) {
        case 0: // play light
          digitalWrite(playLightPin, Mb.R[DEVICES[i]]);
          break;
        case 7: //reset light
          digitalWrite(resetLightPin, Mb.R[DEVICES[i]]);
          break;
        case 8: // servo
          digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
          break;
        default: // valv c
          if (Mb.R[DEVICES[i]]) openValv(i - 1);
          break;
      }

    }
    puzzleSolved = Mb.R[STATE];
    if (Mb.R[STATE]) triggered = Mb.R[STATE];
    gameActivated = Mb.R[ACTIVE];
  }
}

void printSteps() {
  if (!seq_cmp(lastSteps, yourWaterSteps, VALNUM)) {
    Serial.println();
    for (int i = 0; i < VALNUM; i++) {
      Serial.print("Glass "); Serial.print(i); Serial.print(": "); Serial.print(yourWaterSteps[i]); Serial.print(" of  "); Serial.println(waterSteps[i]);
      lastSteps[i] = yourWaterSteps[i];
    }
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
