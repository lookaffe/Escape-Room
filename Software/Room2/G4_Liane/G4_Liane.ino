//Teensy 3.2
#include <MFRC522.h>

#define ONLINE

#define SENNUM  3       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {0, 1, 2}; // SS pins, Configurable, see typical pin layout above
const int actPins[ACTNUM] = {5}; // relay elettrocalamita e pistone
const int devPins[DEVNUM] = {};
//04:E9:E5:06:63:F0
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x92}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 104};                     //This needs to be unique in your network - only one puzzle can have this IP

constexpr uint8_t RST_PIN = 17;          // Configurable, see typical pin layout above



MFRC522 mfrc522[SENNUM];  // Create mfrc522b instances

const int nOfReaders = 3; //numero di RFID abilitati
const int nOfTAG = 3; //numero di TAG abilitati
String currentTAG[nOfReaders]; //Valori del TAG letto
String correctTAG[nOfTAG] = {"0786681131", "0276280131", "01574079131"};

void resetSpec() {
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
  for (uint8_t i = 14; i < nOfReaders+14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  //inizializzazione RFID
  for (uint8_t i = 0; i < SENNUM; i++) {
    mfrc522[i].PCD_Init(i+14, RST_PIN);   // Init mfrc522a
    mfrc522[i].PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
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
  printRegister();
}

void gameUpdate() {
  // Look for new cards
  boolean found[nOfReaders] = {false,false,false};
  for (uint8_t i = 0; i < nOfReaders; i++) {
    mfrc522[i].PCD_Init();
    String readRFID = 0;
    //    Serial.print("Reader "); Serial.print(i);
    //    Serial.print(" - IsNewCardPresent "); Serial.print(mfrc522[i].PICC_IsNewCardPresent());
    //    Serial.print(" - ReadCardSerial "); Serial.println(mfrc522[i].PICC_ReadCardSerial());
    if ( mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
      Serial.print("newtag on reader "); Serial.println(i);
      found[i] = true;
      for (int b = 0 ; b < mfrc522[i].uid.size; b++) readRFID.concat(mfrc522[i].uid.uidByte[b]);
    }
    if (readRFID != currentTAG[i]) {
      currentTAG[i] = readRFID;
    }
    //Halt PICC
    mfrc522[i].PICC_HaltA();
    // Stop encryption on PCD
    mfrc522[i].PCD_StopCrypto1();
  }

  for (uint8_t i = 0; i < nOfReaders; i++) {
    int tag = 10;
    // controllo dei valori dei tag sui lettori RFID
    for (uint8_t y = 0; y < nOfTAG; y++) {
      if (currentTAG[i].equals(correctTAG[y])) {
        tag = y;
      }
    }
    if (found[i]) {
      if (i == tag) {
        sensStatus[i] = 1;
        Serial.print("Correct TAG-" + (String)i + " ");
        Serial.println(currentTAG[i]);
      } else  {
        sensStatus[i] = 2;
        Serial.print("Reader-" + (String)i + " ");
        Serial.println(currentTAG[i]);
      }
    } else sensStatus[i] = 0;
    sensorRegUpdate(i, sensStatus[i]);
    Serial.print("sensStatus["); Serial.print(i); Serial.print("] "); Serial.println(sensStatus[i]);
  } Serial.println();
  puzzleSolved = (sensStatus[0] == 1 && sensStatus[1] == 1 && sensStatus[2] == 1) ? true : false;
  actuatorRegUpdate(0, puzzleSolved);
}
