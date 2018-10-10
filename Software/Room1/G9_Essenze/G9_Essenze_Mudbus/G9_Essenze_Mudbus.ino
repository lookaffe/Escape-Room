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

#define LIMONE 2 // pin della luce limone
#define FUNGO 6 // pin della luce fungo

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x28}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 109};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2};
const int ACTUATORS[ACTNUM] = {}; //nessun attuatore diretto
const int RESET = 999;

// Track the room game state
int puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?

//Used Pins
const int sensPins[SENNUM] = {14, 15}; // // SS pins, Configurable, see typical pin layout above
const int actPins[ACTNUM] = {}; //

int sensStatus[SENNUM] = {0, 0};

const int lightNum = 8;
int lightPin[lightNum] = {0, 1, 2, 3, 4, 5, 6, 7};

bool lightStart = false;
constexpr uint8_t RST_PIN = 16;          // Configurable, see typical pin layout above

MFRC522 mfrc522[SENNUM];  // Create mfrc522b instances


String currentTAG[SENNUM]; //Valori del TAG letto


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
  puzzleSolved = 2;

  //Set light Pin mode
  for (uint8_t a = 0; a < lightNum; a++) {
    pinMode(lightPin[a], OUTPUT);
    digitalWrite(lightPin[a], LOW);
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
  // Su comando da Control Room si attiva la sequenza luci
  if (puzzleSolved == 2) {
    puzzleSolved = 0;
    //flashing();
    lightStart = true;
  }

  Mb.Run();
  listenFromEth();
  if (!triggered && lightStart) {
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
    for (uint8_t b = 0; b < lightNum / 2; b++) {
      digitalWrite(lightPin[b], HIGH); digitalWrite(lightPin[b + lightNum / 2], HIGH);
      delay(fast / h + 1);
      digitalWrite(lightPin[b], LOW); digitalWrite(lightPin[b + lightNum / 2], LOW);
    }
    h++;
  }
  delay(1000);
  digitalWrite(lightPin[LIMONE], HIGH); digitalWrite(lightPin[FUNGO], HIGH);
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
    // controllo dei valori dei tag sui due lettori RFID
    if (currentTAG[0].equals("01836014245") || currentTAG[0].equals("01357614245") || currentTAG[0].equals("01674912245") || currentTAG[0].equals("02154412245")) {
      sensStatus[0] = HIGH;
      Serial.print("Correct TAG-A "); Serial.println(currentTAG[0]);
    } else  {
      sensStatus[0] = LOW;
      Serial.print("TAG-A ");
      Serial.println(currentTAG[0]);
    }
    Mb.R[SENSORS[0]] = sensStatus[0];

    if (currentTAG[1].equals("01996914245") || currentTAG[1].equals("01197914245") || currentTAG[1].equals("02474112245") || currentTAG[1].equals("02317614245")) {
      sensStatus[1] = HIGH;
      Serial.print("Correct TAG-B "); Serial.println(currentTAG[1]);
    } else {
      sensStatus[1] = LOW;
      Serial.print("TAG-B ");
      Serial.println(currentTAG[1]);
    }
    Mb.R[SENSORS[1]] = sensStatus[1];
    digitalWrite(10, LOW);
  }
}

void isPuzzleSolved() {
  puzzleSolved = (sensStatus[0] == 1 && sensStatus[1] == 1) ? true : false;
  Mb.R[STATE] = puzzleSolved;
  if (puzzleSolved == 1) triggered = true; // blocca il controllo RFID se già risolto
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
  triggered = false;
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];
  }
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;

  // reset specifico per questo gioco
  for (uint8_t i = 0; i < lightNum; i++) digitalWrite(lightPin[i], LOW);
  lightStart = false;
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = Mb.R[SENSORS[i]];
  }
  // escluso per questo specifico gioco, assenza di attuatori
  //  triggered = 0;
  //  for (int i = 0; i < ACTNUM ; i++) {
  //    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
  //    triggered = triggered || Mb.R[ACTUATORS[i]];
  //  }

  // controllo specifico per questo gioco (se è 2 avvia le luci)
  triggered = Mb.R[101];
  puzzleSolved = Mb.R[STATE];
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  Serial.println();
  Serial.print("triggered: "); Serial.println(triggered);
  Serial.println();
}
