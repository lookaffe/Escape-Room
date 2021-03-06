// Teensy LC

#include <SPI.h>
#include <Ethernet.h>
#include <MudbusE.h>

#include <Adafruit_SleepyDog.h>

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define SENNUM 6 //total amount of sensors
#define ACTNUM 0 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0xBA}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 105};                       //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6};
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {};
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {23, 22, 21, 20, 19, 18}; //Puzzle number
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {} ;

int sensStatus[SENNUM] = {0, 0, 0, 0, 0, 0};

int val = 100;

//ModbusIP object
MudbusE Mb;

void setup()
{
  Serial.begin(9600);
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

  //Set Pin mode
  pinMode(sensPins[0], INPUT);
  pinMode(sensPins[1], INPUT);
  pinMode(sensPins[2], INPUT);
  pinMode(sensPins[3], INPUT);
  pinMode(sensPins[4], INPUT);
  pinMode(sensPins[5], INPUT);

  Watchdog.enable(4000);
}

void loop()
{
  Watchdog.reset();
  delay(1);
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  int count = 0;
  int iter = 3000;
  for (int y = 0; y < iter; y++) {
    bool puSo = 0;
    for (int i = 0; i < SENNUM ; i++) {
      sensStatus[i] = analogRead(sensPins[i]);
      Mb.R[SENSORS[i]] = sensStatus[i];
    }
    for (int i = 0; i < SENNUM ; i++) {
      int pS = (sensStatus[i] < 100) ? 1 : 0;
      puSo = puSo || pS;
    }
    Serial.print("puSo "); Serial.println(puSo);
    count = count + puSo;
    Serial.print("count "); Serial.println(count);
  }
  if (count == iter) puzzleSolved = true;
}

void isPuzzleSolved() {

  Mb.R[STATE] = puzzleSolved;
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
