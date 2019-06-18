//Teensy 3.2
#include <MFRC522.h>
#include <Servo.h>

//#define ONLINE

#define SENNUM  5       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  1       //total amount of internal devices
#define ALWAYSACTIVE 0  //1 if the game is always active

const int senPins[SENNUM] = {14, 23, 19, 15, 6}; // SS pins1, SS pins2, temperatura, tempo, sportello, servo
const int actPins[ACTNUM] = {5}; // elettrocalamita
const int devPins[DEVNUM] = {0};  // controllo fine gioco

//04:E9:E5:06:63:F0 - 04:E9:E5:07:A4:08
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x07, 0xA4, 0x08}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 113};                     //This needs to be unique in your network - only one puzzle can have this IP

constexpr uint8_t RST_PIN = 17;          // Configurable, see typical pin layout above

MFRC522 mfrc522[SENNUM];  // Create mfrc522b instances

Servo myservo;  // servo instance
int servoPin = 3;

const int nOfReaders = 2; //numero di TAG abilitati
String currentTAG[SENNUM]; //Valori del TAG letto
String correctTAG[nOfReaders] = {"016911615599", "011011119211"};

int tempValue = 0;  // valore pot temperatura
int timeValue = 0;  // valore pot timer

int state = 0;
bool s = 0;
bool checkActivation = false;

//bool sensStatus[SENNUM] = {0,0,0,0,0};

void resetSpec() {
  state = 0;
  resetServo();
  checkActivation = false;
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
  //inizializzazione RFID
  for (uint8_t i = 0; i < nOfReaders; i++) {
    mfrc522[i].PCD_Init(senPins[i], RST_PIN);   // Init mfrc522a
    mfrc522[i].PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  }
  myservo.attach(servoPin);
  resetServo();
}

void loop()
{
  Mb.Run();
  listenFromEth();

  // move the motor to initial position on activation
  if (gameActivated) {
    if (!checkActivation) initServo();
    checkActivation = true;

    if (!triggered) {
      gameUpdate();
      isPuzzleSolved();
    }

    if(deviceRegRead(0)) {
      resetServo();
      deviceRegUpdate(0, 0);
    }
  }
  //printRegister();
}

void gameUpdate() {

  Serial.println(state);
  switch (state) {
    case 0:
      readRFID();
      readPot();
      ovenClosed();
      s = checkStatus();
      actuatorRegUpdate(0, s);
      myDelay(500);
      if (s) state = 1;
      if(Mb.R[STATE]){
        moveServo();
        puzzleSolved = true;      
      }
      break;
    case 1:
      moveServo();
      actuatorRegUpdate(0, 0);
      state = 2;
      myDelay(500);
      puzzleSolved = true;
      break;
    case 2:
      break;
  }
}

void readRFID() {
  // Look for new cards
  for (uint8_t i = 0; i < nOfReaders; i++) {
    mfrc522[i].PCD_Init();
    String readRFID = 0;
    //    Serial.print("Reader "); Serial.print(i);
    //    Serial.print(" - IsNewCardPresent "); Serial.print(mfrc522[i].PICC_IsNewCardPresent());
    //    Serial.print(" - ReadCardSerial "); Serial.println(mfrc522[i].PICC_ReadCardSerial());
    if ( mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
      //      Serial.print("newtag on reader "); Serial.println(i);
      for (int b = 0 ; b < mfrc522[i].uid.size; b++) readRFID.concat(mfrc522[i].uid.uidByte[b]);
    }
    //        Serial.println(readRFID);
    if (readRFID != currentTAG[i]) {
      currentTAG[i] = readRFID;
    }
    //Halt PICC
    mfrc522[i].PICC_HaltA();
    // Stop encryption on PCD
    mfrc522[i].PCD_StopCrypto1();
  }

  for (uint8_t i = 0; i < nOfReaders; i++) {
    boolean found = false;
    int tag = 0;
    // controllo dei valori dei tag sui due lettori RFID
    for (uint8_t y = 0; y < nOfReaders; y++) {
      if (currentTAG[i].equals(correctTAG[y])) {
        found = true;
        tag = y;
        //        Serial.print("Reader "); Serial.print(i); Serial.print(" - tag "); Serial.println(y);
      }
    }
    if (found) {
      if (i == tag) {
        sensStatus[i] = 1;
        //                switch (i) {
        //                  case 0:
        //                    Serial.print("Correct TAG-A ");
        //                    Serial.println(currentTAG[i]);
        //                    break;
        //                  case 1:
        //                    Serial.print("Correct TAG-B ");
        //                    Serial.println(currentTAG[i]);
        //                    break;
        //      }
      } else  {
        sensStatus[i] = 2;
        //              switch (i) {
        //                case 0:
        //                  Serial.print("Reader-A ");
        //                  Serial.println(currentTAG[i]);
        //                  break;
        //                case 1:
        //                  Serial.print("Reader-B ");
        //                  Serial.println(currentTAG[i]);
        //                  break;
        //    }
      }
    } else sensStatus[i] = 0;
    sensorRegUpdate(i, sensStatus[i]);
    //    Serial.print("sensStatus["); Serial.print(i); Serial.print("] "); Serial.println(sensStatus[i]);
  }
  //  Serial.println();
}

void readPot() {
  // read the value from the sensor:
  tempValue = map(analogRead(senPins[2]), 1, 1023, 0, 250);
  // turn the ledPin on
  timeValue = map(analogRead(senPins[3]), 1, 1023, 0, 60);
  //  Serial.print("tempValue " + (String)tempValue + " - timeValue " + (String) timeValue);
  if (tempValue < 215 && tempValue > 205) {
    sensStatus[2] = 1;
    //    Serial.print(" -- TEMP OK");
  } else sensStatus[2] = 0;
  if (timeValue < 47 && timeValue > 43) {
    sensStatus[3] = 1;
    //    Serial.print(" -- TIME OK");
  } else sensStatus[3] = 0;
  //  Serial.println();
  myDelay(100);
  sensorRegUpdate(2, sensStatus[2]);
  sensorRegUpdate(3, sensStatus[3]);
}

void ovenClosed() {
  sensStatus[4] = !digitalRead(senPins[4]); //check in oven is closed
  sensorRegUpdate(4, sensStatus[4]);
}

void initServo(){
  int pos = myservo.read();
  myservo.attach(servoPin);
  for (pos; pos >= 20; pos -= 1)
  {
    myservo.write(pos);
    myDelay(25);
  }
}

void moveServo(){
  myservo.attach(servoPin);
  for (int pos = 20; pos <= 180; pos += 1)
  {
    myservo.write(pos);
    myDelay(25);
  }
}

void resetServo() {
  myservo.detach();
  int pos = myservo.read();
  myservo.attach(servoPin);
  for (pos; pos >= 80; pos -= 1)
  {
    myservo.write(pos);
    myDelay(25);
  }
  myservo.detach();
}

bool checkStatus() {
  bool solv = true;
  bool senSolv;
  Serial.print("sens: ");
  for (int i = 0 ; i < SENNUM; i++) {
    senSolv = false;   
    if(sensStatus[i]==1) senSolv = true;
    Serial.print((String)senSolv + " - ");
    solv = solv && senSolv;
  }
  Serial.println();
  return solv;
}

