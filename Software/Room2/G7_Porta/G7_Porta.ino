//Teensy 3.2

//#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  4       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {21, 20, 19, 18}; // le 4 parti della porta
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 107};                     //This needs to be unique in your network - only one puzzle can have this IP

int hitNum = 7;  // numero di colpi
int hit = 0;
int doorSequence[hitNum] = {3, 3, 4, 4, 1, 1, 2};      //the right sequence
int yourDoorSequence[hitNum] = {0, 0, 0, 0, 0, 0, 0}; // user sequence

unsigned long interrupt_time = 0;

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 300;          // Minimum signal from the piezo to register as a knock
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

void resetSpec() {
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
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
  if (hit < hitNum) {
    knockOnDoor(hit);
    if (millis() - interrupt_time > 3000 || hit == hitNum) {
      Serial.println("CHECK");
      if (seq_cmp(yourDoorSequence, doorSequence, hitNum)) {
        puzzleSolved = true;
      }
      else {
        puzzleSolved = false;
        seq_clear(yourDoorSequence, hitNum);
      }
      hit = 0;
      interrupt_time = millis();
      stateRegUpdate(puzzleSolved);
    }
  }
}

void knockOnDoor(int iter) {
  for (int y = 0; y < 4; y++) {
    int hitted = analogRead(senPins[y]);
    if (hitted > threshold) {
      yourDoorSequence[iter] = y;
      hit++;
      mydelay(knockFadeTime);
    }
    sensorRegUpdate(y, hitted);
    interrupt_time = millis();
    //Serial.print("iter: "); Serial.print(iter); Serial.print(" - pressed: "); Serial.println(b);
    //printHam();
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
    sensorRegUpdate(n, 0);
    //    sensStatus[n] = LOW;
    //    pressed = false;
    //    interrupt_time = millis();
  }
}


