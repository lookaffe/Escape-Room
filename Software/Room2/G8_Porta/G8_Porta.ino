//Teensy 3.2

//#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  4       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  10       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {23, 21, 19, 17}; // le 4 parti della porta
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {0,0,0,0,0,0,0,0,0,0};

//04:E9:E5:08:57:03
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x08, 0x57, 0x03}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 108};                     //This needs to be unique in your network - only one puzzle can have this IP

const int hitNum = 10;  // numero di colpi
int hit = 0;
int doorSequence[hitNum] = {2, 2, 3, 3, 0, 0, 1, 0, 0, 0};      //the right sequence
int yourDoorSequence[hitNum] = {0, 0, 0, 0, 0, 0, 0, 0 , 0, 0}; // user sequence

unsigned long interrupt_time = 0;
unsigned long waiting_time = 3000;

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 400;          // Minimum signal from the piezo to register as a knock
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

void resetSpec() {
  interrupt_time = millis();
  hit = 0;
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
  interrupt_time = millis();
  for (int i = 0; i < hitNum; i++) {
    //doorSequence[i] = deviceRegRead(i);
  }
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  //printRegister();
}

void gameUpdate() {
  if (hit < hitNum) {
    knockOnDoor(hit);
    if (millis() - interrupt_time > waiting_time || hit == hitNum) {
      Serial.println("CHECK");
      if (seq_cmp(yourDoorSequence, doorSequence, hitNum)) {
        puzzleSolved = true;
      }
      else {
        puzzleSolved = false;
        seq_clear(yourDoorSequence, hitNum);
        interrupt_time = millis();
      }
      if (hit == 0)interrupt_time = millis();
      hit = 0;
      stateRegUpdate(puzzleSolved);
    }
  }
  //for(int i = 0; i<hitNum; i++) deviceRegUpdate(i,yourDoorSequence[i]);
}

void knockOnDoor(int iter) {
  for (int y = 0; y < SENNUM; y++) {
    int hitted = analogRead(senPins[y]);
    if (hitted > threshold) {
      yourDoorSequence[iter] = y;
      hit++;
      myDelay(knockFadeTime);
      Serial.println("Hitted " + (String)y);
      interrupt_time = millis();
    }
    sensorRegUpdate(y, hitted);
  }
}

//compare the two sequences
boolean seq_cmp(int *a, int *b, int siz) {
  for (int n = 0; n < siz; n++) if (a[n] != b[n]) return false;
  Serial.println("seq_cmp");
  return true;
}

//clear the sequence
void seq_clear(int *s, int siz) {
  for (int n = 0; n < siz; n++)  {
    s[n] = 0;
  }
  Serial.println("seq_clear");
}


