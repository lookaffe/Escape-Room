//Teensy LC
#include <Wire.h>
#include <pcf8574.h>   // import library

#define ONLINE

#define SENNUM  0       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {}; // m1, m2, m3, m4
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {};
//04:E9:E5:06:65:46
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0x65, 0x46}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 102};                     //This needs to be unique in your network - only one puzzle can have this IP

void resetSpec() {
}

#include <EscapeFunction.h>

pcf8574 mcp0(0x20);//instance
pcf8574 mcp1(0x21);//instance
pcf8574 mcp2(0x22);//instance
pcf8574 mcp3(0x23);//instance

bool letters[32] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool loveWord[32] = {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
bool exitWord[32] = {0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0};
bool helpWord[32] = {1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0};
bool bedWord[32] =  {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
bool wcWord[32] =   {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool tvWord[32] =   {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};
bool singWord[32] = {0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0};

void setup()
{
  setupEscape();
  mcp0.begin();
  mcp0.gpioPinMode(INPUT);
  mcp1.begin();
  mcp1.gpioPinMode(INPUT);
  mcp2.begin();
  mcp2.gpioPinMode(INPUT);
  mcp3.begin();
  mcp3.gpioPinMode(INPUT);
}

void loop()
{
  Mb.Run();
  listenFromEth();
  //if (!triggered) {
    gameUpdate();
    //isPuzzleSolved();
  //}
  printRegister();
}

void gameUpdate() {
  readPins();
  puzzleSolved = word_compare();
  sensorRegUpdate(0, puzzleSolved);
}

void readPins() {
  for (int i = 0 ; i < 8; i++) {
    letters[i] = mcp0.gpioDigitalRead(i);
    //sensorRegUpdate(i+1,letters[i]);
    letters[i + 8] = mcp1.gpioDigitalRead(i);
    //sensorRegUpdate(i+9,letters[i+8]);
    letters[i + 16] = mcp2.gpioDigitalRead(i);
    //sensorRegUpdate(i+17,letters[i+16]);
    letters[i + 24] = mcp3.gpioDigitalRead(i);
    //sensorRegUpdate(i+25,letters[i+24]);
  }
}

int word_compare() {
  if (word_cmp(letters, loveWord)) return 7;
  else if (word_cmp(letters, exitWord)) return 1;
  else if (word_cmp(letters, helpWord)) return 2;
  else if (word_cmp(letters, bedWord)) return 3;
  else if (word_cmp(letters, wcWord)) return 4;
  else if (word_cmp(letters, tvWord)) return 5;
  else if (word_cmp(letters, singWord)) return 6;
  else return 0;
  }

boolean word_cmp(bool *a, bool *b) {
  for (int n = 0; n < 32; n++) if (a[n] != b[n]) return false;
  return true;
}
