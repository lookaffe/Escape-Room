//Teensy LC

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Keypad.h>

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL)

#define SENNUM  1 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  1 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x92}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 111};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1};
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {54}; //il 51 è usato
const int RESTART = 99;
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
int puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
//const int sensPins[SENNUM] = {1}; // tastierino
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {} ;

int sensStatus[SENNUM] = {0};

//specifico per gioco
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad
long date = 0;
long prevPressTime = 0;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//ModbusIP object
Mudbus Mb;

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
  puzzleSolved = 0;
  Mb.R[ACTIVE] = gameActivated;
  Mb.R[DEVICES[0]] = 0;
  //
  prevPressTime = millis();

}

void loop() {
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  Serial.print("Date :"); Serial.println(date);
  printRegister();
}

void gameUpdate() {
  int number = getNumber();
  if (number != 0) {
    if (date == number) {
      puzzleSolved = 1;
    } else Mb.R[DEVICES[0]] = !Mb.R[DEVICES[0]];
  }
}

int getNumber() {
  prevPressTime = millis();
  int num = 0;
  char key = kpd.getKey();
  while (millis() - prevPressTime < 1500) {
    switch (key) {
      case NO_KEY:
        //Mb.Run();
        break;

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        prevPressTime = millis();
        Serial.print(key);
        num = num * 10 + (key - '0');
        Mb.R[SENSORS[0]] = num;
        break;

      case '*': case '#': case 'A': case 'B': case 'C': case 'D':
        Mb.R[SENSORS[0]] = num;
        num = 0;
        Serial.println();
        break;
    }
    key = kpd.getKey();
  }
  return num;
}

void isPuzzleSolved() {
  Mb.R[STATE] = puzzleSolved;
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
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  else {
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
      triggered = Mb.R[STATE];
    }
    gameActivated = Mb.R[ACTIVE];
  }
  date = ((Mb.R[51] * 10000) + (Mb.R[52] * 100) + Mb.R[53] - 2000);
  if(Mb.R[RESTART]) CPU_RESTART;
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
