#include "mac.h"

void setup() {
  delay(2000);
  Serial.begin(57600);
  
  Serial.println("Reading MAC from hardware...");
  read_mac();
  
  Serial.print("MAC: ");
  print_mac();
  Serial.println();
  
  Serial.print("Finished.");
}

void loop() {
  delay(2000);
  Serial.println();
  print_mac();
}

