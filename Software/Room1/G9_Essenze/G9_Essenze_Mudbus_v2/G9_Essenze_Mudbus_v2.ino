// Teensy 3.2

/*
  RFID a (sinistra), tag riconosciuti
  1836014245  1357614245  1674912245  2154412245

  RFID b (destra), tag riconosciuti
  1996914245  1197914245  2474112245  2317614245

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               mfrc522a      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin         Teensy
   ------------------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST         9
   SPI SS      SDA(SS)      10            53        D10        10               10          10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          11
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          12
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          13
*/

#include <SPI.h>
#include <Ethernet.h>
#include <MFRC522.h>
#include <Mudbus.h>

#define SENNUM  2 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  8 //total amount of internal devices

#define LIMONE 2 // pin della luce limone
#define FUNGO 6 // pin della luce fungo

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x28}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 109};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2};
const int ACTUATORS[ACTNUM] = {}; //nessun attuatore diretto
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58};
const int RESET = 100;
const int ACTIVE = 125;

// Track the room game state
int puzzleSolved = false;  // has the puzzle in the room been solved?

//Used Pins
const int sensPins[SENNUM] = {14, 15}; // // SS pins, Configurable, see typical pin layout above
const int actPins[ACTNUM] = {}; //
const int devPins[DEVNUM] = {0, 1, 2, 3, 4, 5, 6, 7};

int sensStatus[SENNUM] = {0, 0};



bool gameActivated = false;
bool started = false;
constexpr uint8_t RST_PIN = 16;          // Configurable, see typical pin layout above

MFRC522 mfrc522[SENNUM];  // Create mfrc522b instances

const int nOfTAG = 16; //numero di TAG abilitati
String currentTAG[SENNUM]; //Valori del TAG letto
String correctTAG[nOfTAG] = {"0221125142218", "0237112144218", "015741148218", "077161136218", "01836014245", "01357614245", "01674912245", "02154412245", "01349135218", "0141176138218", "07792135218", "077119144218", "01996914245", "01197914245", "02474112245", "02317614245"}; //i primi nOfTAG/2 sono del TAG A, gli altri del TAG B

//ModbusIP object
Mudbus Mb;

void setup() {
  // reset for Ethernet Shield
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  delay(1000);
  digitalWrite(9, HIGH); // release the WIZ820io

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  Ethernet.begin(mac, ip);
  delay(5000);
  // disable w5100 SPI

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = 0;

  //Set light Pin mode
  for (uint8_t a = 0; a < DEVNUM; a++) {
    pinMode(devPins[a], OUTPUT);
    digitalWrite(devPins[a], LOW);
  }

  //inizializzazione RFID
  for (uint8_t i = 0; i < SENNUM; i++) {
    mfrc522[i].PCD_Init(sensPins[i], RST_PIN);   // Init mfrc522a
    Serial.println("game");
    mfrc522[i].PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  }
}

void loop() {
  Mb.Run();
  listenFromEth();

  // Su comando da Control Room si attiva la sequenza luci
  if (gameActivated && !started) {
    flashing();
    started = true;
  }

  if (!puzzleSolved && started) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

// sequenza luci
void flashing() {
  uint8_t h = 0;
  int fast = 800;
  while (h < 36) {
    for (uint8_t b = 0; b < DEVNUM / 2; b++) {
      digitalWrite(devPins[b], HIGH); digitalWrite(devPins[b + DEVNUM / 2], HIGH);
      delay(fast / h + 1);
      digitalWrite(devPins[b], LOW); digitalWrite(devPins[b + DEVNUM / 2], LOW);
    }
    h++;
    Mb.Run();
  }
  delay(1000);
  digitalWrite(devPins[LIMONE], HIGH); digitalWrite(devPins[FUNGO], HIGH);
  Mb.R[DEVICES[LIMONE]] = HIGH; Mb.R[DEVICES[FUNGO]] = HIGH;
}


void gameUpdate() {
  // Look for new cards
  for (uint8_t i = 0; i < SENNUM; i++) {
    mfrc522[i].PCD_Init();
    String readRFID = 0;
    if ( mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
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
    // controllo dei valori dei tag sui due lettori RFID
    for (uint8_t y = 0; y < nOfTAG / 2; y++) {
      if (currentTAG[i].equals(correctTAG[y + (i * nOfTAG / 2)])) found = true;
    }
    if (found) {
      sensStatus[i] = HIGH;
      if (!i) {
        Serial.print("Correct TAG-A ");
        Serial.println(currentTAG[i]);
      }
      else {
        Serial.print("Correct TAG-B ");
        Serial.println(currentTAG[i]);
      }
    } else  {
      if (currentTAG[i] != "0") {
        sensStatus[i] = 2;
        if (!i) {
          Serial.print("TAG-A ");
          Serial.println(currentTAG[i]);
        }
        else {
          Serial.print("TAG-B ");
          Serial.println(currentTAG[i]);
        }
      } else sensStatus[i] = LOW;
    }
    Mb.R[SENSORS[i]] = sensStatus[i];
  }
}

void isPuzzleSolved() {
  puzzleSolved = (sensStatus[0] == 1 && sensStatus[1] == 1) ? true : false;
  Mb.R[STATE] = puzzleSolved;
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  digitalWrite(s, trig);
  delay(10);
}

// Resetta il gioco
void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], LOW);
  }
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], LOW);
    Mb.R[DEVICES[i]] = LOW;
  }
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;


  // reset specifico
  gameActivated = false;
  Mb.R[ACTIVE] = gameActivated;
  started = false;
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = Mb.R[SENSORS[i]];
  }
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
  }
  puzzleSolved = Mb.R[STATE];
  gameActivated = Mb.R[ACTIVE];
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
//  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
//  Serial.println();
  Serial.print("ACTIVATION: "); Serial.println(Mb.R[ACTIVE]);
  Serial.println();
}
