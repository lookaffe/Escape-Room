//Arduino Mega

//librerie per comunicazione con CC
#include <SPI.h>
#include <Ethernet.h>
#include "CCModBus.h"
 
#define S1_00 4
#define S1_01 5
#define S1_02 6
#define S1_03 7
#define S1_SOut 8

#define S2_00 28
#define S2_01 29
#define S2_02 30
#define S2_03 31
#define S2_SOut 32

#define S3_00 34
#define S3_01 35
#define S3_02 36
#define S3_03 37
#define S3_SOut 38

CCModBus CCMod;                        //Nuova istanza CCModBus class
byte MyMac[] = {0x04, 0xE9, 0xE5, 0x02, 0xB9, 0x25}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino
IPAddress MyIP(192, 168, 1, 6);                           //This needs to be unique in your network - only one puzzle can have this IP
IPAddress gateway(192, 168, 1, 254);                //This should be changed to be the gateway of your network
IPAddress subnet(255, 255, 255, 0);//ipconfig

IPAddress ClueControlIP (192, 168, 1, 100);                  //change this to the IP of your ClueControl Computer
const int PUZNUM = 8; //Puzzle number


int frequency = 0;

void setup() {
  // Inizializzazione e test funzionamento Ethernet
  Serial.begin(9600);
  Serial.println("Serial interface started");  //Serial interface is for debugging/demo

  Ethernet.begin(MyMac, MyIP, gateway, subnet);
  Serial.println("Ethernet interface started");

  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  CCMod.ClueControlIP = ClueControlIP;

  // print the ClueControl Address:
  Serial.print("ClueControl IP address: ");
  Serial.println(CCMod.ClueControlIP);
  Serial.println();
  
  pinMode(S1_00, OUTPUT);
  pinMode(S1_01, OUTPUT);
  pinMode(S1_02, OUTPUT);
  pinMode(S1_03, OUTPUT);
  pinMode(S1_SOut, INPUT);

  pinMode(S2_00, OUTPUT);
  pinMode(S2_01, OUTPUT);
  pinMode(S2_02, OUTPUT);
  pinMode(S2_03, OUTPUT);
  pinMode(S2_SOut, INPUT);
  
  pinMode(S3_00, OUTPUT);
  pinMode(S3_01, OUTPUT);
  pinMode(S3_02, OUTPUT);
  pinMode(S3_03, OUTPUT);
  pinMode(S3_SOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S1_00,HIGH);
  digitalWrite(S1_01,LOW);

  // Setting frequency-scaling to 20%
  digitalWrite(S2_00,HIGH);
  digitalWrite(S2_01,LOW);

  // Setting frequency-scaling to 20%
  digitalWrite(S3_00,HIGH);
  digitalWrite(S3_01,LOW);
  
}

void loop() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S1_02,LOW);
  digitalWrite(S1_03,LOW);
  // Reading the output frequency
  frequency = pulseIn(S1_SOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //frequency = map(frequency, 25,72,255,0);
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Green filtered photodiodes to be read
  digitalWrite(S1_02,HIGH);
  digitalWrite(S1_03,HIGH);
  // Reading the output frequency
  frequency = pulseIn(S1_SOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //frequency = map(frequency, 30,90,255,0);
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Blue filtered photodiodes to be read
  digitalWrite(S1_02,LOW);
  digitalWrite(S1_03,HIGH);
  // Reading the output frequency
  frequency = pulseIn(S1_SOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //frequency = map(frequency, 25,70,255,0);
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.println("  ");
  delay(100);
}

void puzzleSolved(boolean sol){
  CCMod.SetTrigger(PUZNUM, sol);   //when pin5 is pulled low, set coil to true in ClueControl
}

//Check to see if a new command has been received by looking at the
  //CCMod.CCValue.
  //  if (CCMod.CCValue == 12)         //Could also use a switch statement instead of the IF statements
  //  {
  //    digitalWrite(6, HIGH);        //if command 12 is received, turn the LED on
  //    CCMod.CCValue = 0;        //very important to set the value back to 0, otherwise the IF statement
  //    //will fire again
  //    Serial.println("Message received from ClueControl - 12");
  //  }
  //
  //  if (CCMod.CCValue == 13)
  //  {
  //    digitalWrite(6, LOW);         //if command 13 is received, turn the LED off
  //    CCMod.CCValue = 0;
  //  }

  //Check pin 5, send to CC if it goes low.  Once it goes low, block it from
  //sending again until the pin first goes high

  //  else
  //  {
  //    Serial.println("Pin 5 deactivated, sending 6-false");
  //    Pin5Hold = false;               //Set the hold flag so this IF doesn't trigger again and again
  //    CCMod.SetTrigger(6, false);    //This one command is all that is needed to send a value to ClueControl.
  //
  //
  //  }
