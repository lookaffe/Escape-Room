/* Teensy 3.2
 *  
  RFID a (sinistra), tag riconosciuti
  1836014245
  1357614245
  1674912245
  2154412245

  RFID b (destra), tag riconosciuti
  1996914245
  1197914245
  2474112245
  2317614245



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
#include <MFRC522.h>

constexpr uint8_t SS_PINa = 14;         // Configurable, see typical pin layout above

constexpr uint8_t RST_PIN = 16;          // Configurable, see typical pin layout above
constexpr uint8_t SS_PINb = 15;         // Configurable, see typical pin layout above

MFRC522 mfrc522a(SS_PINa, RST_PIN);  // Create mfrc522a instance
MFRC522 mfrc522b(SS_PINb, RST_PIN);  // Create mfrc522b instance

String currentTAGa = "";
String currentTAGb = "";

void setup() {
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522a.PCD_Init();   // Init mfrc522a
  mfrc522a.PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
  mfrc522b.PCD_Init();   // Init mfrc522a
  mfrc522b.PCD_DumpVersionToSerial();  // Show details of PCD - mfrc522a Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  currentTAGa = "";
  currentTAGb = "";
  // Look for new cards
  if ( mfrc522a.PICC_IsNewCardPresent() || mfrc522b.PICC_IsNewCardPresent()) {

    // Select one of the cards
    if (mfrc522a.PICC_ReadCardSerial()) {
      for (int i = 0; i < mfrc522a.uid.size; i++) {
        currentTAGa.concat(mfrc522a.uid.uidByte[i]);
      }
      if (currentTAGa.equals("1836014245") || currentTAGa.equals("1357614245") || currentTAGa.equals("1674912245") || currentTAGa.equals("2154412245")) {
      Serial.print("currTAGa "); Serial.println(currentTAGa);
      }
    }
    Serial.print("currentTAGa "); Serial.println(currentTAGa);

    // Dump debug info about the card; PICC_HaltA() is automatically called
    //mfrc522a.PICC_DumpToSerial(&(mfrc522a.uid));
    //dump_byte_array(mfrc522a.uid.uidByte, mfrc522a.uid.size);

    if (mfrc522b.PICC_ReadCardSerial()) {
      for (int i = 0; i < mfrc522b.uid.size; i++) {
        currentTAGb.concat(mfrc522b.uid.uidByte[i]);
      }
      if (currentTAGb.equals("1996914245") || currentTAGb.equals("1197914245") || currentTAGb.equals("2474112245") || currentTAGb.equals("2317614245")) {
      Serial.print("currTAGb "); Serial.println(currentTAGb);
      }
    }
Serial.print("currentTAGb "); Serial.println(currentTAGb);
  }
}
