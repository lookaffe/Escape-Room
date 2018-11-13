//Teensy LC

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  2 //total amount of sensors
#define ACTNUM  1 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x99}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 181};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2};
const int ACTUATORS[ACTNUM] = {101};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool panic = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {16, 22}; // switch, door
const int actPins[ACTNUM] = {21}; // relay
const int devPins[DEVNUM] ={};

int sensStatus[SENNUM] = {LOW, LOW};

Bounce button0 = Bounce(sensPins[0], 100); //Pin button

//ModbusIP object
Mudbus Mb;

void setup()
{
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
  panic = false;
  Mb.R[ACTIVE] = gameActivated;

  //Set Pin mode
  pinMode(sensPins[0], INPUT);
  pinMode(sensPins[1], INPUT);
  pinMode(actPins[0], OUTPUT);
  digitalWrite(actPins[0], HIGH); //Open on LOW

}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPanic();
  }
  printRegister();
}

void gameUpdate() {
  button0.update();

  if (button0.risingEdge()) panic = true;
  Mb.R[SENSORS[0]] = panic;
  Mb.R[SENSORS[1]] = digitalRead(sensPins[1]);
}

void isPanic() {
  trigger(0, panic);
  Mb.R[STATE] = panic;
  triggered = panic;
}

// Azione su ricezione comando "trigger"
void trigger(int actPin, boolean trig) {
  Mb.R[ACTUATORS[actPin]] = trig;
  triggered = trig;
  digitalWrite(actPins[actPin], !trig);
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
  panic = false;
  Mb.R[STATE] = panic;
  Mb.R[RESET] = LOW;
  if (!ALWAYSACTIVE) {
    gameActivated = false;
    Mb.R[ACTIVE] = gameActivated;
  }
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
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
  panic = Mb.R[STATE];
    if (Mb.R[STATE]) {
      for (int i = 0; i < ACTNUM ; i++) {
        trigger(i, Mb.R[STATE]);
      }
      triggered = Mb.R[STATE];
    }
  gameActivated = Mb.R[ACTIVE];
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
  Serial.print("trig: "); Serial.println(triggered);
  Serial.println();
}

