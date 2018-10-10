// ARDUINO MEGA (non usare pin 20, 21, 50, 51, 52, 53)

#include "CapacitiveSensor.h"

/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/
const int LEVEL = 24;
int minLevel = 400;
CapacitiveSensor   mano01 = CapacitiveSensor(A8, A9);
CapacitiveSensor   mano02 = CapacitiveSensor(A10, A11);
CapacitiveSensor   mano03 = CapacitiveSensor(A12, A13);
CapacitiveSensor   mano04 = CapacitiveSensor(A14, A15);
CapacitiveSensor   mano05 = CapacitiveSensor(A4, A5);
CapacitiveSensor   mano06 = CapacitiveSensor(A6, A7);
CapacitiveSensor   mano07 = CapacitiveSensor(49, 48);
CapacitiveSensor   mano08 = CapacitiveSensor(47, 46);
CapacitiveSensor   mano09 = CapacitiveSensor(45, 44);
CapacitiveSensor   mano10 = CapacitiveSensor(43, 42);
CapacitiveSensor   mano11 = CapacitiveSensor(41, 40);
CapacitiveSensor   mano12 = CapacitiveSensor(39, 38);
CapacitiveSensor   piede01 = CapacitiveSensor(37, 36);
CapacitiveSensor   piede02 = CapacitiveSensor(35, 34);
CapacitiveSensor   piede03 = CapacitiveSensor(33, 32);
CapacitiveSensor   piede04 = CapacitiveSensor(31, 30);
CapacitiveSensor   piede05 = CapacitiveSensor(29, 28);
CapacitiveSensor   piede06 = CapacitiveSensor(27, 26);
CapacitiveSensor   piede07 = CapacitiveSensor(25, 24);
CapacitiveSensor   piede08 = CapacitiveSensor(23, 22);
CapacitiveSensor   piede09 = CapacitiveSensor(3, 2);
CapacitiveSensor   piede10 = CapacitiveSensor(19, 18);
CapacitiveSensor   piede11 = CapacitiveSensor(17, 16);
CapacitiveSensor   piede12 = CapacitiveSensor(15, 14);

long push = 0;
int sequence[LEVEL] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long pushed[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int relayPin = 9;

void setup()
{
  mano01.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano02.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano03.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano04.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano05.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano06.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano07.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano08.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano09.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano10.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano11.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano12.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede01.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede02.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede03.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede04.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede05.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede06.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede07.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede08.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede09.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede10.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede11.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede12.set_CS_AutocaL_Millis(0xFFFFFFFF);

  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

}

void loop()
{
  pushed[0] =  mano01.capacitiveSensor(30);
  pushed[1] =  mano02.capacitiveSensor(30);
  pushed[2] =  mano03.capacitiveSensor(30);
  pushed[3] =  mano04.capacitiveSensor(30);
  pushed[4] =  mano05.capacitiveSensor(30);
  pushed[5] =  mano06.capacitiveSensor(30);
  pushed[6] =  mano07.capacitiveSensor(30);
  pushed[7] =  mano08.capacitiveSensor(30);
  pushed[8] =  mano09.capacitiveSensor(30);
  pushed[9] =  mano10.capacitiveSensor(30);
  pushed[10] =  mano11.capacitiveSensor(30);
  pushed[11] =  mano12.capacitiveSensor(30);
  pushed[12] =  piede01.capacitiveSensor(30);
  pushed[13] =  piede02.capacitiveSensor(30);
  pushed[14] =  piede03.capacitiveSensor(30);
  pushed[15] =  piede04.capacitiveSensor(30);
  pushed[16] =  piede05.capacitiveSensor(30);
  pushed[17] =  piede06.capacitiveSensor(30);
  pushed[18] =  piede07.capacitiveSensor(30);
  pushed[19] =  piede08.capacitiveSensor(30);
  pushed[20] =  piede09.capacitiveSensor(30);
  pushed[21] =  piede10.capacitiveSensor(30);
  pushed[22] =  piede11.capacitiveSensor(30);
  pushed[23] =  piede12.capacitiveSensor(30);


  Serial.println();
  for (int x = 0; x < LEVEL; x++) {
    if (pushed[x] > minLevel) { //pushed[x] > 400)
      yourSequence[x] = 1;
    } else yourSequence[x] = 0;
    Serial.print(yourSequence[x]); Serial.print(" ");
  }
  if (seq_cmp(yourSequence, sequence)) {
    Serial.print("OK");
    digitalWrite(relayPin, LOW);
  } else digitalWrite(relayPin, HIGH);


  //  //Serial.print(millis() - start);        // check on performance in milliseconds
  //
  //  Serial.print("Mano"); Serial.print(total2);                 // print sensor output 1 - this is also the value you can use to use this in other projects
  //  Serial.print(" - "); Serial.print("Piede"); Serial.println(total1);
  // arbitrary delay to limit data to serial port
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < LEVEL; n++) if (a[n] != b[n]) return false;
  return true;
}
