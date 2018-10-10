//Teensy LC

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>

#define SENNUM  1 //total amount of sensors
#define ACTNUM  1 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0xC2}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 102};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1};
const int ACTUATORS[ACTNUM] = {101};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
//Used Pins
const int sensPins[SENNUM] = {16}; // Reed
const int actPins[ACTNUM] = {21}; // Relay
const int devPins[DEVNUM] = {} ;

int sensStatus[SENNUM] = {LOW};

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
  puzzleSolved = false;

  //Set Pin mode
  pinMode(actPins[0], OUTPUT);
  pinMode(sensPins[0], INPUT_PULLUP);
  digitalWrite(actPins[0], HIGH); //Open on LOW
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
  sensStatus[0] = digitalRead(sensPins[0]); //Leggo il valore del Reed
  Mb.R[SENSORS[0]] = !sensStatus[0];
  puzzleSolved = !sensStatus[0];
  Mb.R[STATE] = puzzleSolved;
}

void isPuzzleSolved() {
  trigger(actPins[0], puzzleSolved);
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  digitalWrite(s, !trig);
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
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = Mb.R[SENSORS[i]];
  }
  triggered=0;
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
    triggered = triggered || Mb.R[ACTUATORS[i]];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
  }
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS ");Serial.print(i);Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS ");Serial.print(i);Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  for (int i = 0; i < DEVNUM; i++) {
    Serial.print("DEVNUM "); Serial.print(i); Serial.print(" (reg "); Serial.print(DEVICES[i]); Serial.print(") - val:  "); Serial.println(Mb.R[DEVICES[i]]);
  }
  Serial.print("RESET (reg "); Serial.print(RESET);Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  Serial.println();
  Serial.print("triggered: "); Serial.println(triggered);
  Serial.println();
}



