//Teensy 3.2

#include <Bounce.h>
#include <MFRC522.h>

#define ONLINE // if you are working without ethernet comment this define

#define SENNUM  3       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  1       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {8, 7, 23}; // pulsante1, pulsante1, RFID
const int actPins[ACTNUM] = {0}; // ElettroCalamita
const int devPins[DEVNUM] = {1}; // counter
//04:E9:E5:06:E1:27
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x27}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 109};                     //This needs to be unique in your network - only one puzzle can have this IP

bool spark = false; // verifica la pressione alternata dei pulsanti
bool lastSpark = false;
int sparkCount = 0; // conta l'alternanza dei pulsanti
int minSpark = 16;  // numero minimo di volte in cui i pulsanti si alternano
elapsedMillis sparkTime;  // intervallo tra due pressioni
elapsedMillis elapSpeed;    // tempo tra una pressione e la successiva
const long minSpeed = 1000; // tempo minimo tra due pressioni
const long minCount = 20;    // minimo numero di pressioni ripetute

Bounce button0 = Bounce(8, 10);
Bounce button1 = Bounce(7, 10);  // 10 = 10 ms debounce time

constexpr uint8_t RST_PIN = 17;          // Configurable, see typical pin layout above

MFRC522 mfrc522;  // Create mfrc522b instances

String currentTAG; //Valori del TAG letto
String correctTAG[3] = {"023712421943", "01612390219", "0643090219" };

void resetSpec() {
  sparkCount = 0;
  elapSpeed = 0;
}

#include <EscapeFunction.h>

void setup() {
  setupEscape();
  elapSpeed = 0;

  //inizializzazione RFID
  mfrc522.PCD_Init(senPins[2], RST_PIN);   // Init mfrc522a
  Serial.println("game");
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  buttonUpdate();
  readRFID();
  Serial.print("Spark: " + (String)spark + " - lastSpark: " + (String)lastSpark);
  if (spark != lastSpark) {
    lastSpark = spark;
    sparkCount++;
    elapSpeed = 0;
  }
  Serial.println(" - count: " + (String)sparkCount);
 deviceRegUpdate(0, sparkCount);
  if (elapSpeed > minSpeed) {
    sparkCount = 0;
    currentTAG = "0";
  }
  if ((sparkCount > minCount) && ((currentTAG==correctTAG[0]) || (currentTAG==correctTAG[1]) || (currentTAG==correctTAG[2]))) {
    puzzleSolved = true;
    actuatorRegUpdate(0,1); // apri l'elettrocalamita
  }
}

void buttonUpdate() {
  button0.update();
  if (button0.risingEdge()) {
    spark = true;
    sensorRegUpdate(0, 1);
  } else sensorRegUpdate(0, 0);
  button1.update();
  if (button1.risingEdge()) {
    spark = false;
    sensorRegUpdate(1, 1);
  }else sensorRegUpdate(1, 0);
}

void readRFID() {
  mfrc522.PCD_Init();
  String readedRFID = 0;
  if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (int b = 0 ; b < mfrc522.uid.size; b++) readedRFID.concat(mfrc522.uid.uidByte[b]);
  }
  if ((readedRFID != currentTAG) && (readedRFID!="0")) {
    currentTAG = readedRFID;
  }
  sensorRegUpdate(2, currentTAG.toInt());
  Serial.println("TAG : " + currentTAG);
  //Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  sensorRegUpdate(2, currentTAG.toInt());
}

