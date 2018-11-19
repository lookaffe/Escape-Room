


/*
  Example using the SparkFun HX711 breakout board with a scale
  By: Nathan Seidle
  SparkFun Electronics
  Date: November 19th, 2014
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This is the calibration sketch. Use it to determine the calibration_factor that the main example uses. It also
  outputs the zero_factor useful for projects that have a permanent mass on the scale in between power cycles.

  Setup your scale and start the sketch WITHOUT a weight on the scale
  Once readings are displayed place the weight on the scale
  Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
  Use this calibration_factor on the example sketch

  This example assumes pounds (lbs). If you prefer kilograms, change the Serial.print(" lbs"); line to kg. The
  calibration factor will be significantly different but it will be linearly related to lbs (1 lbs = 0.453592 kg).

  Your calibration factor may be very positive or very negative. It all depends on the setup of your scale system
  and the direction the sensors deflect from zero state
  This example code uses bogde's excellent library: https://github.com/bogde/HX711
  bogde's library is released under a GNU GENERAL PUBLIC LICENSE
  Arduino pin 2 -> HX711 CLK
  3 -> DOUT
  5V -> VCC
  GND -> GND

  Rosso: E +
  Nero: E -
  Verde: A -
  Bianco: A +



*/

#include "HX711.h"

#define DOUT1  2
#define CLK  3
#define DOUT2  4
//#define CLK2  5
#define DOUT3  6
//#define CLK3  7

#define calibration_factor1 -393400.0
#define calibration_factor2 -380000.0
#define calibration_factor3 -399200.0
#define libToGr 453.592

HX711 scale1(DOUT1, CLK);
HX711 scale2(DOUT2, CLK);
HX711 scale3(DOUT3, CLK);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0
  scale2.set_scale();
  scale2.tare(); //Reset the scale to 0
  scale3.set_scale();
  scale3.tare(); //Reset the scale to 0

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  long zero_factor3 = scale3.read_average(); //Get a baseline reading
  Serial.print("Zero factor 1: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(zero_factor1);
  Serial.print(" - Zero factor 2: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(zero_factor2);
  Serial.print(" - Zero factor 3: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor3);
}

void loop() {

  scale1.set_scale(calibration_factor1); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor
  scale3.set_scale(calibration_factor3); //Adjust to this calibration factor

  Serial.print("Reading 1: ");
  Serial.print(scale1.get_units()*libToGr, 1);
  Serial.print(" - ");
  Serial.print("Reading 2: ");
  Serial.print(scale2.get_units()*libToGr, 1);
  Serial.print(" - ");
  Serial.print("Reading 3: ");
  Serial.print(scale3.get_units()*libToGr, 1);
  Serial.println(" g");
}

