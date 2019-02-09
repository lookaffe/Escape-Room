/*
  Playing with the HC-SR04 Ultrasonic Sensor
  3/26/17 by Brian Patton
  Feel free to do whatever you want with this code example
*/


//#define ONLINE

#define SENNUM  2       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  2       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {21, 23}; // Ultrasuono alto, Ultrasuono basso
const int actPins[ACTNUM] = {2}; // elettrovalvola nuvola
const int devPins[DEVNUM] = {0, 1}; // luce alta, luce bassa

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 106};                     //This needs to be unique in your network - only one puzzle can have this IP

void resetSpec() {
}

#include <EscapeFunction.h>

byte trigPin[2] = {20, 22};  // Trigger is connected to this pin
byte echoPin[2] = {21, 23};   // Echo is connected to this pin
float echoTime;       // Variable to store echo pulse time
float CMs;            // Variable to carry Cm values
float Inches;         // Variable to carry Inch values
float distance;       // Variable to hold calculated distance
//************************************************************
//                Setup
//************************************************************
void setup() {
  setupEscape();
  pinMode(trigPin[0], OUTPUT);   // Set pin to Output
  pinMode(trigPin[1], OUTPUT);   // Set pin to Output
  pinMode(echoPin[0], INPUT);    // Set pin to Intput
  pinMode(echoPin[1], INPUT);    // Set pin to Intput
  digitalWrite(trigPin[0], LOW); // Start the tigger pin off as low
  digitalWrite(trigPin[1], LOW); // Start the tigger pin off as low
  Serial.begin(9600);         // Open the Serial Port
}
//************************************************************
//                Main Loop
//************************************************************
void loop() {
  Mb.Run();
  listenFromEth();
  for (int i = 0; i < 2; i++) {
    distance = echoCM(i);    // Call non-void function to get the distance in cm
    Serial.println("Distance in Cms " + (String)i + "= " + (String)distance);    // print data
    delay(300);             // Delay 1/2 second
  }
  printRegister();
}
//************************************************************
//                Calculate distance 1 in cm
//************************************************************
float echoCM(int n) {
  digitalWrite(trigPin[n], LOW);  // Make sure the pin starts off low
  delay(1);                    // Min between triggers is 50us so let's do 1000us
  digitalWrite(trigPin[n], HIGH); // Set trigger pin High for....
  delayMicroseconds(10);       // 10 microseconds.
  digitalWrite(trigPin[n], LOW);  // Set it low again

  echoTime = pulseIn(echoPin[n], HIGH);  //Collect the echo pulse
  CMs = echoTime / 58;                //Conversion in Data Sheet
  return CMs;                         // Return the calculated value
}
