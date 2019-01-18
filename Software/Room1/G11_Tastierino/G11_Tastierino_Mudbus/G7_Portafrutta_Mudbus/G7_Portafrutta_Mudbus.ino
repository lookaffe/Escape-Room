//Arduino Uno
/* aa
  Rosso: E +
  Nero: E -
  Verde: A -
  Bianco: A +
*/
#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include "HX711.h"
#include <avr/wdt.h>

#define CLK  3
#define DOUT1  2
#define DOUT2  4
#define DOUT3  5

#define calibration_factor1 -393400.0
#define calibration_factor2 -380000.0
#define calibration_factor3 -399200.0
#define libToGr 453.592


#define SENNUM  3 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x08}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 107};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3};
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {};
const int RESTART = 99;
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {DOUT1, DOUT2, DOUT3}; // weigth sensors
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {} ;

int sensStatus[SENNUM] = {LOW};

// specifico
HX711 scale1(DOUT1, CLK);
HX711 scale2(DOUT2, CLK);
HX711 scale3(DOUT3, CLK);

const float minWeigth = 75.0;
const float maxWeigth = 95.0;

bool check = false;
int count = 0;

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
  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0
  scale3.set_scale();
  scale3.tare(); //Reset the scale to 0

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  long zero_factor3 = scale3.read_average(); //Get a baseline reading

  wdt_enable(WDTO_8S);
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
  int w1 = 0;
  int w2 = 0;
  int w3 = 0;
  int mis = 1;
  scale1.set_scale(calibration_factor1); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor
  scale3.set_scale(calibration_factor3); //Adjust to this calibration factor
  for (int i = 0; i < mis; i++) {
    w1 = w1 + scale1.get_units() * libToGr;
    w2 = w2 + scale2.get_units() * libToGr;
    w3 = w3 + scale3.get_units() * libToGr;
  }
  float weigth1 = w1 / mis; (weigth1 < 0) ? weigth1 = 0 : weigth1;
  float weigth2 = w2 / mis; (weigth2 < 0) ? weigth2 = 0 : weigth2;
  float weigth3 = w3 / mis; (weigth3 < 0) ? weigth3 = 0 : weigth3;

  //  Serial.print("Reading 1: "); Serial.print(weigth1, 1);
  Mb.R[SENSORS[0]] = weigth1;
  //  Serial.print(" - ");
  //  Serial.print("Reading 2: "); Serial.print(weigth2, 1);
  Mb.R[SENSORS[1]] = weigth2;
  //  Serial.print(" - ");
  //  Serial.print("Reading 3: "); Serial.print(weigth3, 1);
  Mb.R[SENSORS[2]] = weigth3;
  //  Serial.print(minWeigth); Serial.print(" < "); Serial.print(weigth1); Serial.print(" < "); Serial.println(maxWeigth);
  if (((minWeigth < weigth1) && (weigth1 < maxWeigth)) && ((minWeigth < weigth2) && (weigth2 < maxWeigth)) && ((minWeigth < weigth3) && (weigth3 < maxWeigth))) puzzleSolved = true;

}

void isPuzzleSolved() {
  check = puzzleSolved;
  if (puzzleSolved) {
    count++;
    if (count > 13) {
      if (check == puzzleSolved) {
        Mb.R[STATE] = check;
        triggered = check;
      }
      count = 0;
      check = false;
    }
    Serial.print("ps "); Serial.print("- "); Serial.println(puzzleSolved);
    Serial.print("check "); Serial.print("- "); Serial.println(check);
    Serial.print("count "); Serial.print("- "); Serial.println(count);
  }
  else {
    count = 0;
  }
}

void setTare(){
  //Set Pin mode
  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0
  scale3.set_scale();
  scale3.tare(); //Reset the scale to 0

  Mb.Run();
  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  Mb.Run();
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  Mb.Run();
  long zero_factor3 = scale3.read_average(); //Get a baseline reading
  Mb.Run();
}

// action on "trigger"
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
  setTare();
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
  if(Mb.R[RESTART]) asm volatile (" jmp 0 ");
  wdt_reset();
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

