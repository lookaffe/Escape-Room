//Teensy LC

//librerie per comunicazione con CC
#include <SPI.h>
#include <Ethernet.h>


const int pinSwitch = 6; //Pin Reed
const int pinRelay = 12;
int StatoSwitch = 0;
void setup()
{
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
    //delay(1000);
    puzzleSolved(true);
  }
  else
  {
    puzzleSolved(false);
  }
}

void puzzleSolved(boolean sol) {
  digitalWrite(pinRelay, !sol);
  Serial.println(!sol);
  delay(10);
}
