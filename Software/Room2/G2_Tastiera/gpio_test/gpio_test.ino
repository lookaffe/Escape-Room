#include <Wire.h>
#include <pcf8574.h>   // import library

// 04:E9:E5:06:65:46

pcf8574 mcp0(0x20);//instance
pcf8574 mcp1(0x21);//instance
pcf8574 mcp2(0x22);//instance

void setup(){
  mcp0.begin();
  mcp0.gpioPinMode(INPUT);
    mcp1.begin();
  mcp1.gpioPinMode(INPUT);
      mcp2.begin();
  mcp2.gpioPinMode(INPUT);
}

void loop(){
  Serial.print("Pin 0-0 : " + (String)mcp0.gpioDigitalRead(0));
  Serial.print(" - Pin 1-0 : " + (String)mcp1.gpioDigitalRead(0));
  Serial.println(" - Pin 2-7 : " + (String)mcp2.gpioDigitalRead(7));
}
