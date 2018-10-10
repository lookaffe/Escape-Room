//Teensy LC

//librerie per comunicazione con CC
#include <SPI.h>
#include <Ethernet.h>
#include "CCModBus.h"

CCModBus CCMod;                        //Nuova istanza CCModBus class
byte MyMac[] = {0x04, 0xE9, 0xE5, 0x02, 0xB9, 0x25}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino
IPAddress MyIP(192, 168, 1, 6);                           //This needs to be unique in your network - only one puzzle can have this IP
IPAddress gateway(192, 168, 1, 254);                //This should be changed to be the gateway of your network
IPAddress subnet(255, 255, 255, 0);//ipconfig

IPAddress ClueControlIP (192, 168, 1, 100);                  //change this to the IP of your ClueControl Computer
const int PUZNUM = 2; //Puzzle number

const int pinSwitch = 16; //Pin Reed
const int pinRelay = 21;
int StatoSwitch = 0;
void setup()
{
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
  
  pinMode(pinRelay, OUTPUT); //Imposto i PIN
  pinMode(pinSwitch, INPUT_PULLUP);
  digitalWrite(pinRelay, HIGH);
  Serial.begin(9600);
}
void loop()
{
  StatoSwitch = digitalRead(pinSwitch); //Leggo il valore del Reed
  if (StatoSwitch == LOW)
  {
    delay(1000);
    puzzleSolved(true);
  }
  else
  {
    puzzleSolved(false);
  }
}

void puzzleSolved(boolean sol) {
  CCMod.SetTrigger(PUZNUM, sol);   //
  digitalWrite(pinRelay, !sol);
  Serial.print("UNO se è alto: "); Serial.println(sol);
  delay(10);
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
