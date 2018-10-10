


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



  Most any pin on the Arduino Uno will be compatible with DOUT/CLK.

  The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.


// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library
HX711_ADC LoadCell(4, 5); // parameters: dt pin, sck pin<span data-mce-type="bookmark" style="display: inline-block; width: 0px; overflow: hidden; line-height: 0;" class="mce_SELRES_start"></span>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // 0x27 is the i2c address of the LCM1602 IIC v1 module (might differ)
void setup() {
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(999.0); // calibration factor for load cell => strongly dependent on your individual setup
  lcd.begin(16, 2); // begins connection to the LCD module
  lcd.backlight(); // turns on the backlight
}
void loop() {
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  lcd.setCursor(0, 0); // set cursor to first row
  lcd.print("Weight[g]:"); // print out to LCD
  lcd.setCursor(0, 1); // set cursor to secon row
  lcd.print(i); // print out the retrieved value to the second row
}
*/

#include "HX711.h"

#define CLK1  2
#define DOUT1  3
#define CLK2  4
#define DOUT2  5
#define CLK3  6
#define DOUT3  7


HX711 scale1(DOUT1, CLK1);
HX711 scale2(DOUT2, CLK2);
HX711 scale3(DOUT3, CLK3);

float calibration_factor = -1550; //-7050 worked for my 440lb max scale setup

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
  Serial.println(zero_factor1);
  Serial.print(" - Zero factor 2: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(zero_factor2);
  Serial.print(" - Zero factor 3: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.print(zero_factor3);
}

void loop() {

  scale1.set_scale(calibration_factor); //Adjust to this calibration factor
  scale2.set_scale(calibration_factor); //Adjust to this calibration factor
  scale3.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading 1: ");
  Serial.print(scale1.get_units(), 1);
  Serial.print(" - ");
  Serial.print("Reading 2: ");
  Serial.print(scale2.get_units(), 1);
  Serial.print(" - ");
  Serial.print("Reading 3: ");
  Serial.print(scale3.get_units(), 1);
  Serial.println();

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 100;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 100;
  }
}

