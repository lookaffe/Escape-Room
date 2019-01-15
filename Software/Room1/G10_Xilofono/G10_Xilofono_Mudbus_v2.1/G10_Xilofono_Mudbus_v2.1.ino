
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
#include <Servo.h>

#define SENNUM  14 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM 9 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

// per questo gioco------
#define VALNUM 6
#define HAMNUM 6
// ----------------------

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE4}; //Dipende da ogni disposnoteivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 110};                           //This needs to be unique in your network - only one puzzle can have this IP

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

// per questo gioco------
const int playLightPin = 21;
const int playButtonPin = 19;
boolean playState = false;

const int resetLightPin = 33;
const int resetButtonPin = 35;
int resetState = false;

const int valvPins[VALNUM] = {0, 2, 4, 6, 8, 7};
const int valvButtonPins[VALNUM] = {17, 15, 22, 20, 18, 16};
int valvButtonState[VALNUM] = {0, 0, 0, 0, 0, 0};

const int hammButtonPins[HAMNUM] = { 25, 24, 26, 28, 30, 32};
int hammButtonState[HAMNUM] = {0, 0, 0, 0, 0, 0};

const long fluxTime = 200; // 2900 per 0,5 cl

int waterSteps[VALNUM] = {8, 6, 5, 10, 3, 1};  //quantità d'acqua per le note giuste Verde, Blu, Rosa, Rosso, Giallo, Nero
int yourWaterSteps[VALNUM] = {0, 0, 0, 0, 0, 0};
int lastSteps[VALNUM] = {0, 0, 0, 0, 0, 0}; //only for debugging
boolean waterSolved = false;

const int noteNum = 14;
int note = 0; // number of iteration

int hammSequence[noteNum] = {0, 0, 4, 4, 5, 5, 4, 3, 3, 2, 2, 1, 1, 0};      //the right sequence
int yourHammSequence[noteNum] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence
int lastHamm[noteNum] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //only for debugging

boolean pressed = false;
unsigned long interrupt_time = 0;

Servo glassesServo;
int servoPos = 0;

Bounce hammButton0 = Bounce(25, 100);
Bounce hammButton1 = Bounce(24, 100);  // 10 = 10 ms debounce time
Bounce hammButton2 = Bounce(26, 100);  // which is appropriate for
Bounce hammButton3 = Bounce(28, 100);  // most mechanical pushbuttons
Bounce hammButton4 = Bounce(30, 100);
Bounce hammButton5 = Bounce(32, 100);

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

  glassesServo.attach(23);
  firstServo();
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
  if (!waterSolved) {
    fillTheGlasses();
    waterSolved = seq_cmp(yourWaterSteps, waterSteps, VALNUM);
    digitalWrite(resetLightPin, !waterSolved);
    Mb.R[DEVICES[7]] = !waterSolved;
    digitalWrite(playLightPin, waterSolved);
    Mb.R[DEVICES[0]] = waterSolved;
    if (waterSolved) Mb.R[SENSORS[0]] = waterSolved;
  }
  else {
    playInstrument();
  }
}

void fillTheGlasses() {
  //Serial.print("resetState-pre :"); Serial.print(resetState);
  resetState = analogRead(resetButtonPin);
  Serial.print(" - post :"); Serial.print(resetState);
  Serial.print(" - real :"); Serial.println(digitalRead(resetButtonPin));
  if (analogRead(resetButtonPin))  emptiesGlasses();
  for (int i = 0; i < VALNUM; i++) {
    valvButtonState[i] = digitalRead(valvButtonPins[i]);
    if (!valvButtonState[i]) {
      openValv(i);
      yourWaterSteps[i] = yourWaterSteps[i] + 1;
      Mb.R[SENSORS[i + 1]] = yourWaterSteps[i];
    }
  }
  interrupt_time = millis();
}

void openValv(int v) {
  digitalWrite(valvPins[v], HIGH); // HIGH se uso SSRelay
  delay(fluxTime);
  digitalWrite(valvPins[v], LOW);
  delay(200);
}

void emptiesGlasses() {
  for (int i = 0; i < VALNUM; i++) {
    yourWaterSteps[i] = 0;
    Mb.R[SENSORS[i + 1]] = yourWaterSteps[i];
  }
  for (servoPos = 0; servoPos <= 170; servoPos++) { // goes from 0 degrees to 180 degrees
    //Serial.print("servoPos :"); Serial.println(servoPos); // in steps of 1 degree
    glassesServo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (servoPos = 170; servoPos >= 0; servoPos--) { // goes from 180 degrees to 0 degrees
    Serial.print("servoPos :"); Serial.println(servoPos);
    glassesServo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
}

void firstServo() {
  for (servoPos = 170; servoPos >= 0; servoPos--) { // goes from 180 degrees to 0 degrees
    Serial.print("servoPos :"); Serial.println(servoPos);
    glassesServo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 15ms for the servo to reach the position
  }
}

void buttonUpdate(int ite) {
  hammButton0.update();
  hammButton1.update();
  hammButton2.update();
  hammButton3.update();
  hammButton4.update();
  hammButton5.update();


  if (hammButton0.fallingEdge()) {
    fallingEdgeAction(0, ite);
  }
  if (hammButton1.fallingEdge()) {
    fallingEdgeAction(1, ite);
  }
  if (hammButton2.fallingEdge()) {
    fallingEdgeAction(2, ite);
  }
  if (hammButton3.fallingEdge()) {
    fallingEdgeAction(3, ite);
  }
  if (hammButton4.fallingEdge()) {
    fallingEdgeAction(4, ite);
  }
  if (hammButton5.fallingEdge()) {
    fallingEdgeAction(5, ite);
  }
}

void fallingEdgeAction(int b, int iter) {
  pressed = true;
  yourHammSequence[iter] = b;
  //Mb.R[SENSORS[b]] = hammButtonState[b];
  interrupt_time = millis();
  Serial.print("iter: "); Serial.print(iter); Serial.print(" - pressed: "); Serial.println(b);
  printHam();
  note++;
}

void playInstrument() {
  if (!pressed) playState = !digitalRead(playButtonPin);
  if (playState) {
    Serial.println("play pressed");
    Mb.R[SENSORS[0]]++;
  }
  if (note < noteNum) {
    buttonUpdate(note);
    if (pressed) {
      if (millis() - interrupt_time > 3000 || note == noteNum) {
        Serial.println("too slow");
        pressed = false;
        note = 0;
        interrupt_time = millis();
        if (seq_cmp(yourHammSequence, hammSequence, noteNum)) {
          puzzleSolved = true;
          triggered = 1;
        }
        else {
          puzzleSolved = false;
          seq_clear(yourHammSequence, noteNum);
          printHam();
        }
        Mb.R[STATE] = puzzleSolved;
      }
    }
    digitalWrite(playLightPin, !pressed);
    Mb.R[DEVICES[0]] = !pressed;
  }
  else {
    seq_clear(yourHammSequence, noteNum);
    printHam();
    pressed = false;
    note = 0;
  }
}

//compare the two sequences
boolean seq_cmp(int *a, int *b, int siz) {
  for (int n = 0; n < siz; n++) if (a[n] != b[n]) return false;
  return true;
}

//clear the sequence
void seq_clear(int *s, int siz) {
  for (int n = 0; n < siz; n++)  {
    s[n] = 0;
    sensStatus[n] = LOW;
    Mb.R[SENSORS[n]] = sensStatus[n];
    pressed = false;
    interrupt_time = millis();
  }
}

void isPuzzleSolved() {
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

  // reset per il gioco
  waterSolved = 0;
  emptiesGlasses();
  seq_clear(yourHammSequence, noteNum);
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
          if (Mb.R[DEVICES[i]]) openValv(i);
          break;
      }
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

void printSteps() {
  if (!seq_cmp(lastSteps, yourWaterSteps, VALNUM)) {
    Serial.println();
    for (int i = 0; i < VALNUM; i++) {
      Serial.print("Glass "); Serial.print(i); Serial.print(": "); Serial.print(yourWaterSteps[i]); Serial.print(" of  "); Serial.println(waterSteps[i]);
      lastSteps[i] = yourWaterSteps[i];
    }
  }
}

void printHam() {
  if (!seq_cmp(lastHamm, yourHammSequence, noteNum)) {
    Serial.println();
    for (int i = 0; i < noteNum; i++) {
      Serial.print("Played hammer "); Serial.print(i); Serial.print(": "); Serial.print(yourHammSequence[i]); Serial.print(" instead  "); Serial.println(hammSequence[i]);
      lastHamm[i] = yourHammSequence[i];
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
