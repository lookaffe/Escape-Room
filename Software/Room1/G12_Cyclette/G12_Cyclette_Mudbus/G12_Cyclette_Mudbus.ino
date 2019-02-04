//Teensy 3.2

#define PRINT

#include <SPI.h>
#include <Ethernet.h>
#include <MudbusE.h>  // Mudbus modificata con controllo su connessione ethernet
#include <Bounce.h>

#include <Adafruit_SleepyDog.h>

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define SENNUM  6 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x2B}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 112};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6};
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;
const int ACTIVE = 124;

int puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

const int sensPins[SENNUM] = {0, 1, 2, 3, 4, 5}; // Capacitor
const int actPins[ACTNUM] = {}; // relay
const int devPins[DEVNUM] = {} ;

int sensStatus[SENNUM] = {1, 1, 1, 1, 1, 1};

//specifici per il gioco
unsigned long passedtime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long prevRotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long rotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
int rpm[SENNUM] = {0, 0, 0, 0, 0, 0};
volatile boolean printTime = true;
volatile int cycle = 0;
boolean firstTime = true;

int players = 0;
const int maxRPM = 110;
const int minRPM = 90;

//ModbusIP object
MudbusE Mb;

void setup() {
  // reset for Ethernet Shield
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  for (int i = 0; i < 100; i++) {
    delay(10);
    Watchdog.reset();
  }
  digitalWrite(9, HIGH); // release the WIZ820io

  Ethernet.begin(mac, ip);
  for (int i = 0; i < 100; i++) {
    delay(50);
    Watchdog.reset();
  }

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = false;
  Mb.R[ACTIVE] = gameActivated;

  // Configure the pins for input mode with pullup resistors.
  for (int i = 0; i < SENNUM ; i++) {
    for (int i = 0; i < SENNUM; i++) {
      pinMode(sensPins[i], INPUT); //Sets hallsensor as input
      passedtime[i] = millis();
      prevRotationTime[i] = millis();
    }
  }
  Watchdog.enable(4000);
}

void loop() {
  Watchdog.reset();
  delay(1);
  Mb.Run();
  //Serial.println(players);
  listenFromEth();
  if (triggered != 1) {
    gameUpdate();
    isPuzzleSolved();
  }
#ifdef PRINT
  printRegister();
#endif
}

void gameUpdate() {
  if (firstTime) first();
  for (int i = 0; i < SENNUM; i++) {
    detachInterrupt(i);
    (millis() - prevRotationTime[i] > 3000) ? rotationTime[i] = 0 : rotationTime[i];
    switch (i) {
      case 0:
        attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
        break;
      case 1:
        attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
        break;
      case 4:
        attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
        break;
      case 5:
        attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);
        break;
    }
    if (rotationTime[i]) rpm[i] = (int)60000 / rotationTime[i];
    else rpm[i] = 0;
    Mb.R[SENSORS[i]] = rpm[i];
  }

#ifdef PRINT
  if (printTime) {
    for (int y = 0; y < SENNUM; y++) {
      Serial.print("Cyc: ");
      Serial.print(y); //Print out result to monitor
      Serial.print(" - RPM=");
      Serial.print(rpm[y]); //Print out result to monitor
      Serial.print(" - Prev Rot Time=");
      Serial.print(prevRotationTime[y]); //Print out result to monitor
      Serial.print(" - Millis=");
      Serial.println(millis()); //Print out result to monitor
    }
    Serial.print("puzzleSolved: "); Serial.println(puzzleSolved);
    printTime = false;
    Serial.println();
  }
#endif
}

void isr_0()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[0] = millis() - prevRotationTime[0];
  prevRotationTime[0] = millis();
  cycle = 0;
  printTime = true;
}

void isr_1()
{
  rotationTime[1] = millis() - prevRotationTime[1];
  prevRotationTime[1] = millis();
  cycle = 1;
  printTime = true;
}

void isr_2()
{
  rotationTime[2] = millis() - prevRotationTime[2];
  prevRotationTime[2] = millis();
  cycle = 2;
  printTime = true;
}

void isr_3()
{
  rotationTime[3] = millis() - prevRotationTime[3];
  prevRotationTime[3] = millis();
  cycle = 3;
  printTime = true;
}

void isr_4()
{
  rotationTime[4] = millis() - prevRotationTime[4];
  prevRotationTime[4] = millis();
  cycle = 4;
  printTime = true;
}

void isr_5()
{
  rotationTime[5] = millis() - prevRotationTime[5];
  prevRotationTime[5] = millis();
  cycle = 5;
  printTime = true;
}

void first() {
  firstTime = false;
  attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
  attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
  attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
  attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
  attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
  attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);
  Serial.println("Interrupt all ");
}

void isPuzzleSolved() {
  int rpmTot = 0;

  for (int p = 0; p < SENNUM; p++) {
    ((millis() - prevRotationTime[p]) > 3000) ? rpm[p] = 0 : rpm[p];
    rpmTot += rpm[p];
  }
  int rpmMed = (int) rpmTot / players;
  Mb.R[7] = rpmMed;
  //Serial.print("rpmMed: "); Serial.print(rpmMed);   Serial.print(" - ");
  if (rpmMed < maxRPM) {
    //if (rpmMed < minRPM) {
    puzzleSolved = 0;
    //} else puzzleSolved = 2;
  } else puzzleSolved = 1;

  Mb.R[STATE] = puzzleSolved;
  //Serial.print(" - puzzleSolved: "); Serial.println(puzzleSolved);
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
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;
  if (!ALWAYSACTIVE) {
    gameActivated = false;
    Mb.R[ACTIVE] = gameActivated;
  }

  // reset specifico per il gioco
  for (int i = 0; i < SENNUM ; i++) {
    passedtime[i] = 0;
    prevRotationTime[i] = 0;
    rotationTime[i] = 0;
    rpm[i] = 0;

  }
  printTime = true;
  cycle = 0;
  firstTime = true;
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
  players = Mb.R[51];


  //  // controllo se la connessione ethernet è attiva, altrimenti resetto
  if (!Mb.isConnected()) CPU_RESTART;
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
