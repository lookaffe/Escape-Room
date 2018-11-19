//Teensy 3.2

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Keypad.h>

#define SENNUM  1 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  0 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 111};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const int ACTUATORS[ACTNUM] = {101};
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 24}; // Capacitor
const int actPins[ACTNUM] = {39}; // relay
const int devPins[DEVNUM] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;

int sensStatus[SENNUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
int date = 11281;
long prevPressTime = 0;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//ModbusIP object
Mudbus Mb;

void setup() {
  Serial.begin(9600);
  prevPressTime = millis();
}

void loop() {
  int vv = getNumber();
  (date == vv) ? Serial.println("Yes") : Serial.println("No");
}

int getNumber() {
  prevPressTime = millis();
  int num = 0;
  char key = kpd.getKey();
  while (millis() - prevPressTime < 3000) {
    switch (key) {
      case NO_KEY:
        break;

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        prevPressTime = millis();
        Serial.print(key);
        num = num * 10 + (key - '0');
        break;

      case '*':case '#':case 'A':case 'B':case 'C':case 'D':
        num = 0;
        Serial.println();
        break;
    }
    key = kpd.getKey();
  }
  return num;
}
