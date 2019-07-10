//Teensy LC
#include <MFRC522.h>

#define ONLINE

#define SENNUM  1       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {23}; // SS pins, Configurable, see typical pin layout above
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {};
//04:E9:E5:06:DA:92
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x92}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 112};                     //This needs to be unique in your network - only one puzzle can have this IP

constexpr uint8_t RST_PIN = 17;          // Configurable, see typical pin layout above

MFRC522 mfrc522[SENNUM];  // Create mfrc522b instances

const int nOfReaders = 1; //numero di TAG abilitati
String currentTAG[SENNUM]; //Valori del TAG letto
String correctTAG[nOfReaders] = {"0723820211"};

void resetSpec() {
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
  //inizializzazione RFID
  for (uint8_t i = 0; i < nOfReaders; i++) {
    mfrc522[i].PCD_Init(senPins[0], RST_PIN);   // Init mfrc522a
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
  for (uint8_t i = 0; i < SENNUM; i++) {
    mfrc522[i].PCD_Init();
    String readRFID = 0;
    //    Serial.print("Reader "); Serial.print(i);
    //    Serial.print(" - IsNewCardPresent "); Serial.print(mfrc522[i].PICC_IsNewCardPresent());
    //    Serial.print(" - ReadCardSerial "); Serial.println(mfrc522[i].PICC_ReadCardSerial());
    if ( mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
      Serial.print("newtag on reader "); Serial.println(i);
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

  for (uint8_t i = 0; i < SENNUM; i++) {
    boolean found = false;
    int tag = 0;
    // controllo dei valori dei tag sui lettori RFID
    for (uint8_t y = 0; y < nOfReaders; y++) {
      if (currentTAG[i].equals(correctTAG[y])) {
        found = true;
        tag = y;
        Serial.print("Reader "); Serial.print(i); Serial.print(" - tag "); Serial.println(y);
      }
    }
    if (found) {
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
  puzzleSolved = (sensStatus[0] == 1) ? true : false;
}
