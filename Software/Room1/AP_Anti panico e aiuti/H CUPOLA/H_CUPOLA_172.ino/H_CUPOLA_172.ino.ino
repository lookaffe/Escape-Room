//Teensy LC

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  1 //total amount of sensors
#define ACTNUM  0 //total amount of actuators

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x95}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 172};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1};
const int ACTUATORS[ACTNUM] = {};
const int RESET = 999;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?

//Used Pins
const int sensPins[SENNUM] = {16}; // switch
//const int actPins[ACTNUM] = {39}; // relay

int sensStatus[SENNUM] = {LOW};


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
  puzzleSolved = false;

  //Set Pin mode
  pinMode(sensPins[0], INPUT);
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
  button0.update();

  if (button0.fallingEdge()) puzzleSolved = true;
  //if (button0.risingEdge()) puzzleSolved = false;
  Mb.R[SENSORS[0]] = puzzleSolved;
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
  //  for (int i = 0; i < ACTNUM ; i++) {
  //    trigger(actPins[i], LOW);
  //  }
  triggered = false;
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];
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
  triggered = false;
  //  for (int i = 0; i < ACTNUM ; i++) {
  //    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
  //    triggered = triggered || Mb.R[ACTUATORS[i]];
  //  }
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  Serial.println();
  Serial.print("triggered: "); Serial.println(triggered);
  Serial.println();
}


