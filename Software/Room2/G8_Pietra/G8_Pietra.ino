//Teensy LC

#include <Bounce.h>

//#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  2       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {20, 21}; // pulsanti, acqua, fiaccola, ingranaggio
const int actPins[ACTNUM] = {5}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 108};                     //This needs to be unique in your network - only one puzzle can have this IP

bool spark = false; // verifica la pressione alternata dei pulsanti
bool lastSpark = false;
int sparkCount = 0; // conta l'alternanza dei pulsanti
int minSpark = 16;  // numero minimo di volte in cui i pulsanti si alternano
elapsedMillis sparkTime;  // intervallo tra due pressioni

void resetSpec() {
}

#include <EscapeFunction.h>

Bounce button0 = Bounce(20, 100);
Bounce button1 = Bounce(21, 100);  // 10 = 10 ms debounce time

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
  //printRegister();
}

void gameUpdate() {
  buttonUpdate();
  if (spark != lastSpark) {
    lastSpark = spark;
    sparkCount++;
  }
  Serial.println("Spark: " + (String)spark + " - lastSpark: " + (String)lastSpark + " - count: " + (String)sparkCount);
}

void buttonUpdate() {
  button0.update();
  button1.update();

  if (button0.fallingEdge()) {
    spark = true;
  }
  if (button1.fallingEdge()) {
    spark = false;
  }
}



