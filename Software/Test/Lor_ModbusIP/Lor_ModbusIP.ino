/*
  Modbus-Arduino Example - Lamp (Modbus IP)
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>

  // The media access control (ethernet hardware) address for the shield
  uint8_t mac[] = { 0x04, 0xE9, 0xE5, 0x06, 0xDA, 0xC2 }; //28 con teensy, 58 con Arduino
  // The IP address for the shield
  uint8_t ip[] = {10, 0, 0, 101}; //101 Teensy, 111 Arduino
  
//Modbus Registers Offsets (0-9999)
const int RELAY1 = 100;
const int SWITCH_ISTS = 1;
//Used Pins
const int relayPin = 16;
const int switchPin = 3;

//ModbusIP object
Mudbus Mb;

void setup() {
  Serial.begin(9600);

  
  Ethernet.begin(mac, ip); //Avoid pins 4,10,11,12,13 when using ethernet shield on Arduino and 9,10,11,12,13 on Teensy
  delay(5000);
  
  //Set relayPin mode
  pinMode(relayPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
}

void loop() {

  //Call once inside loop() - all magic here
  Mb.Run();

  Serial.print("Button pressed: "); Serial.print(digitalRead(switchPin)); Serial.print(" - ");
  //Attach switchPin to SWITCH_ISTS register
  Mb.R[SWITCH_ISTS] = digitalRead(switchPin);
  Serial.print("Relay open: "); Serial.println(Mb.R[RELAY1]);
  //Attach relayPin to RELAY1 register
  digitalWrite(relayPin, Mb.R[RELAY1]);
}
