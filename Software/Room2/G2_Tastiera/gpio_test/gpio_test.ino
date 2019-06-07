#include <Wire.h>
#include <pcf8574.h>   // import library

// 04:E9:E5:06:65:46
pcf8574 mcp0(0x20);//instance
pcf8574 mcp1(0x21);//instance
pcf8574 mcp2(0x22);//instance
pcf8574 mcp3(0x23);//instance

bool letters[32] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool loveWord[32] = {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
bool exitWord[32] = {0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0};
bool helpWord[32] = {1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0};
bool bedWord[32] =  {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
bool wcWord[32] =   {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool tvWord[32] =   {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};
bool singWord[32] = {0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0};

void setup() {
  mcp0.begin();
  mcp0.gpioPinMode(INPUT);
  mcp1.begin();
  mcp1.gpioPinMode(INPUT);
  mcp2.begin();
  mcp2.gpioPinMode(INPUT);
  mcp3.begin();
  mcp3.gpioPinMode(INPUT);
}

void loop() {
  
  for (int i = 0 ; i < 8; i++) {
    letters[i] = mcp0.gpioDigitalRead(i);
    letters[i+8] = mcp1.gpioDigitalRead(i);
    letters[i+16] = mcp2.gpioDigitalRead(i);
    letters[i+24] = mcp3.gpioDigitalRead(i);
  }
  Serial.print("letters ");
  for (int y = 0; y<32; y++){
    Serial.print(" - ");
    Serial.print(letters[y]);
  }
  Serial.println();

  if(word_cmp(letters,loveWord)) Serial.println("love");
  if(word_cmp(letters,exitWord)) Serial.println("exit");
  if(word_cmp(letters,helpWord)) Serial.println("help");
  if(word_cmp(letters,bedWord)) Serial.println("bed");
  if(word_cmp(letters,wcWord)) Serial.println("wc");
  if(word_cmp(letters,tvWord)) Serial.println("tv");
  if(word_cmp(letters,singWord)) Serial.println("sing");
}

boolean word_cmp(bool *a, bool *b){
     for (int n=0;n<32;n++) if (a[n]!=b[n]) return false;
     return true;
}
