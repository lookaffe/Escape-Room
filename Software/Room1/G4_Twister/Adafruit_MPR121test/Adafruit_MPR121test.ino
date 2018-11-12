#include <Wire.h>
#include "Adafruit_MPR121.h"

#define HANDS 8
#define SAMPLE 50

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap_hands = Adafruit_MPR121();

uint16_t dataMatrix[SAMPLE][HANDS];
float capValue[HANDS] = {0, 0, 0, 0, 0, 0, 0, 0};
bool capTouch[HANDS] = {0, 0, 0, 0, 0, 0, 0, 0};

const int thres = 120;
uint8_t row = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Adafruit MPR121 cap_handsacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap_hands.begin(0x5A)) {
    Serial.println("MPR121 1 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 1 found!");

  for (int i = 0; i < HANDS; i++) {
    for (int j = 0; j < SAMPLE; j++) {
      dataMatrix[i][j] = 0;
    }
  }
}

void loop() {

  if (row == SAMPLE) {
    checkTouch();
    printValues();
    row = 0;
  }
  for (int col = 0; col < HANDS; col++) {
    dataMatrix[row][col] = cap_hands.filteredData(col);
  }
  row++;
}

void checkTouch() {
  for (int i = 0; i < HANDS; i++) {
    float capAVG = 0;
    for (int j = 0; j < SAMPLE; j++) {
      capAVG = capAVG + dataMatrix[j][i];
    }
    capValue[i] = capAVG / SAMPLE;
    if ( capValue[i] < thres) {
      capTouch[i] = 1;
    } else capTouch[i] = 0;
  }
}

void printValues() {
  Serial.print("Filt: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS; i++) {
    Serial.print(capTouch[i]); Serial.print("\t");
  }
  Serial.println();

  Serial.print("Push: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS; i++) {
    Serial.print(capValue[i]); Serial.print("\t");
  }
  Serial.println();
}

