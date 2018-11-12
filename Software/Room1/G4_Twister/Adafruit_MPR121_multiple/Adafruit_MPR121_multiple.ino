#include <Wire.h>
#include "Adafruit_MPR121.h"

#define HANDS_FEET 12

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap_hands = Adafruit_MPR121();
Adafruit_MPR121 cap_feet = Adafruit_MPR121();

uint16_t dataHandsMatrix[HANDS_FEET][HANDS_FEET];
float capHandsValue[HANDS_FEET] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool capHandsTouch[HANDS_FEET] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint16_t dataFeetMatrix[HANDS_FEET][HANDS_FEET];
float capFeetValue[HANDS_FEET] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool capFeetTouch[HANDS_FEET] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int thres = 65;
uint8_t row = 0;


void setup() {
  Serial.begin(9600);

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap_hands.begin(0x5A)) {
    Serial.println("MPR121 H not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 H found!");

  if (!cap_feet.begin(0x5B)) {
    Serial.println("MPR121 F not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 F found!");
}

void loop() {
  if (row == HANDS_FEET) {
    checkTouch();
    printValues();
    row = 0;
  }
  for (int col = 0; col < HANDS_FEET; col++) {
    dataHandsMatrix[row][col] = cap_hands.filteredData(col);
    dataFeetMatrix[row][col] = cap_feet.filteredData(col);
  }
  row++;
}

void checkTouch() {
  for (int i = 0; i < HANDS_FEET; i++) {
    float capHandsAVG = 0;
    float capFeetAVG = 0;
    for (int j = 0; j < HANDS_FEET; j++) {
      capHandsAVG = capHandsAVG + dataHandsMatrix[j][i];
      capFeetAVG = capFeetAVG + dataFeetMatrix[j][i];
    }
    capHandsValue[i] = capHandsAVG / HANDS_FEET;
    capFeetValue[i] = capFeetAVG / HANDS_FEET;
    if ( capHandsValue[i] < thres) {
      capHandsTouch[i] = 1;
    } else capHandsTouch[i] = 0;
    if ( capFeetValue[i] < thres) {
      capFeetTouch[i] = 1;
    } else capFeetTouch[i] = 0;
  }
}

void printValues() {
  Serial.print("Hand: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS_FEET; i++) {
    Serial.print(capHandsTouch[i]); Serial.print("\t");
  }
  Serial.println();

  Serial.print("Push: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS_FEET; i++) {
    Serial.print(capHandsValue[i]); Serial.print("\t");
  }
  Serial.println();
    Serial.print("Foot: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS_FEET; i++) {
    Serial.print(capFeetTouch[i]); Serial.print("\t");
  }
  Serial.println();

  Serial.print("Push: "); Serial.print("\t");
  for (uint8_t i = 0; i < HANDS_FEET; i++) {
    Serial.print(capFeetValue[i]); Serial.print("\t");
  }
  Serial.println();
}

