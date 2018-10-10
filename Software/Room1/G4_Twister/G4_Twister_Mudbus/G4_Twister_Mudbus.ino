// ARDUINO MEGA (non usare pin 20, 21, 50, 51, 52, 53)

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include "CapacitiveSensor.h"

#define SENNUM  24 //total amount of sensors
#define ACTNUM  2 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0x04, 0x04}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 104};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
const int ACTUATORS[ACTNUM] = {101};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?

//Used Pins
// Sensor  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
// Reg  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
const int sensPins[SENNUM] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47}; // Capacitor
const int actPins[ACTNUM] = {17, 18}; // relay
const int devPins[DEVNUM] = {} ;

long sensStatus[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
int sequence[SENNUM] = {0, 0, 0, 0, 0, 1, 0, 0, 0,  0,  0,  1,  1,  0,  0,  0,  0,  1,  1,  0,  0,  0,  1,  0};
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int minLevel = 400;

//CapacitiveSensor manoPiede[SENNUM];
//Define each capacitor sensor
CapacitiveSensor   mano01 = CapacitiveSensor(22, sensPins[0]);
CapacitiveSensor   mano02 = CapacitiveSensor(22, sensPins[1]);
CapacitiveSensor   mano03 = CapacitiveSensor(22, sensPins[2]);
CapacitiveSensor   mano04 = CapacitiveSensor(22, sensPins[3]);
CapacitiveSensor   mano05 = CapacitiveSensor(22, sensPins[4]);
CapacitiveSensor   mano06 = CapacitiveSensor(22, sensPins[5]);
CapacitiveSensor   mano07 = CapacitiveSensor(22, sensPins[6]);
CapacitiveSensor   mano08 = CapacitiveSensor(22, sensPins[7]);
CapacitiveSensor   mano09 = CapacitiveSensor(22, sensPins[8]);
CapacitiveSensor   mano10 = CapacitiveSensor(22, sensPins[9]);
CapacitiveSensor   mano11 = CapacitiveSensor(22, sensPins[10]);
CapacitiveSensor   mano12 = CapacitiveSensor(22, sensPins[11]);
CapacitiveSensor   piede01 = CapacitiveSensor(22, sensPins[12]);
CapacitiveSensor   piede02 = CapacitiveSensor(22, sensPins[13]);
CapacitiveSensor   piede03 = CapacitiveSensor(22, sensPins[14]);
CapacitiveSensor   piede04 = CapacitiveSensor(22, sensPins[15]);
CapacitiveSensor   piede05 = CapacitiveSensor(22, sensPins[16]);
CapacitiveSensor   piede06 = CapacitiveSensor(22, sensPins[17]);
CapacitiveSensor   piede07 = CapacitiveSensor(22, sensPins[18]);
CapacitiveSensor   piede08 = CapacitiveSensor(22, sensPins[19]);
CapacitiveSensor   piede09 = CapacitiveSensor(22, sensPins[20]);
CapacitiveSensor   piede10 = CapacitiveSensor(22, sensPins[21]);
CapacitiveSensor   piede11 = CapacitiveSensor(22, sensPins[22]);
CapacitiveSensor   piede12 = CapacitiveSensor(22, sensPins[23]);

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


  //for (int i = 0; i< SENNUM; i++){
  //  manoPiede[i] = = CapacitiveSensor(24, sensPins[i]);
  //  manoPiede[i].set_CS_AutocaL_Millis(0xFFFFFFFF);
  //}
  mano01.set_CS_AutocaL_Millis(0x00000FFF);
  mano02.set_CS_AutocaL_Millis(0x00000FFF);
  mano03.set_CS_AutocaL_Millis(0x00000FFF);
  mano04.set_CS_AutocaL_Millis(0x00000FFF);
  mano05.set_CS_AutocaL_Millis(0x00000FFF);
  mano06.set_CS_AutocaL_Millis(0x00000FFF);
  mano07.set_CS_AutocaL_Millis(0x00000FFF);
  mano08.set_CS_AutocaL_Millis(0x00000FFF);
  mano09.set_CS_AutocaL_Millis(0x00000FFF);
  mano10.set_CS_AutocaL_Millis(0x00000FFF);
  mano11.set_CS_AutocaL_Millis(0x00000FFF);
  mano12.set_CS_AutocaL_Millis(0x00000FFF);
  piede01.set_CS_AutocaL_Millis(0x00000FFF);
  piede02.set_CS_AutocaL_Millis(0x00000FFF);
  piede03.set_CS_AutocaL_Millis(0x00000FFF);
  piede04.set_CS_AutocaL_Millis(0x00000FFF);
  piede05.set_CS_AutocaL_Millis(0x00000FFF);
  piede06.set_CS_AutocaL_Millis(0x00000FFF);
  piede07.set_CS_AutocaL_Millis(0x00000FFF);
  piede08.set_CS_AutocaL_Millis(0x00000FFF);
  piede09.set_CS_AutocaL_Millis(0x00000FFF);
  piede10.set_CS_AutocaL_Millis(0x00000FFF);
  piede11.set_CS_AutocaL_Millis(0x00000FFF);
  piede12.set_CS_AutocaL_Millis(0x00000FFF);

  //Set relay Pin mode
  pinMode(actPins[0], OUTPUT);
  pinMode(actPins[1], OUTPUT);
  digitalWrite(actPins[0], HIGH);
  digitalWrite(actPins[1], HIGH);

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
  //  for (int i = 0; i < SENNUM; i++) {
  //    sensStatus[i] =  manoPiede[i].capacitiveSensor(30);
  //  }
  sensStatus[0] =  mano01.capacitiveSensor(30);
  sensStatus[1] =  mano02.capacitiveSensor(30);
  sensStatus[2] =  mano03.capacitiveSensor(30);
  sensStatus[3] =  mano04.capacitiveSensor(30);
  sensStatus[4] =  mano05.capacitiveSensor(30);
  sensStatus[5] =  mano06.capacitiveSensor(30);
  sensStatus[6] =  mano07.capacitiveSensor(30);
  sensStatus[7] =  mano08.capacitiveSensor(30);
  sensStatus[8] =  mano09.capacitiveSensor(30);
  sensStatus[9] =  mano10.capacitiveSensor(30);
  sensStatus[10] =  mano11.capacitiveSensor(30);
  sensStatus[11] =  mano12.capacitiveSensor(30);
  sensStatus[12] =  piede01.capacitiveSensor(30);
  sensStatus[13] =  piede02.capacitiveSensor(30);
  sensStatus[14] =  piede03.capacitiveSensor(30);
  sensStatus[15] =  piede04.capacitiveSensor(30);
  sensStatus[16] =  piede05.capacitiveSensor(30);
  sensStatus[17] =  piede06.capacitiveSensor(30);
  sensStatus[18] =  piede07.capacitiveSensor(30);
  sensStatus[19] =  piede08.capacitiveSensor(30);
  sensStatus[20] =  piede09.capacitiveSensor(30);
  sensStatus[21] =  piede10.capacitiveSensor(30);
  sensStatus[22] =  piede11.capacitiveSensor(30);
  sensStatus[23] =  piede12.capacitiveSensor(30);

  for (int i = 0; i < SENNUM; i++) Mb.R[SENSORS[i]] = sensStatus[i];


  //Serial.println();
  for (int x = 0; x < SENNUM; x++) {
    if (sensStatus[x] > minLevel) { //sensStatus[x] > 400)
      yourSequence[x] = 1;
    } else yourSequence[x] = 0;
    //Serial.print(yourSequence[x]); Serial.print(" ");
  }
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < SENNUM; n++) if (a[n] != b[n]) return false;
  return true;
}

void isPuzzleSolved() {
  puzzleSolved = seq_cmp(yourSequence, sequence) ? true : false ;
  trigger(actPins[0], !puzzleSolved);
  trigger(actPins[1], !puzzleSolved);
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

    // reset specifico per il gioco
    yourSequence[i] = LOW;
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
  triggered = 0;
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
    triggered = triggered || Mb.R[ACTUATORS[i]];
  }
    for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
  }
}

void printRegister() {
  //  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  //  for (int i = 0; i < ACTNUM; i++) {
  //    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  //  }
  //  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  //  Serial.println();
  //  Serial.print("triggered: "); Serial.println(triggered);
  delay(1000);
  Serial.write(12);

//  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
//  for (int i = 0; i < SENNUM; i++) {
//    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
//  }
//  for (int i = 0; i < ACTNUM; i++) {
//    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
//  }
//  for (int i = 0; i < DEVNUM; i++) {
//    Serial.print("DEVNUM "); Serial.print(i); Serial.print(" (reg "); Serial.print(DEVICES[i]); Serial.print(") - val:  "); Serial.println(Mb.R[DEVICES[i]]);
//  }
//  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
//  Serial.println();
//  Serial.print("triggered: "); Serial.println(triggered);
//  Serial.println();
}
