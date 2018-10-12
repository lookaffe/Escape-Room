
// TEENSY 3.5
/*
 * Pin Tasto PLAY 19
 * Pin Tasto RESET 35
 * Pin Luce PLAY 21
 * Pin Luce RESET 33
 * Pin Pulsanti Elettrovalvole 17, 15, 22, 20, 18, 16
 * Pin Pulsanti Martelletti 25, 24, 26, 28, 30, 32
 * Pin Relay Elettrovalvole 0, 2, 4, 6, 8, 7
 * Pin Servo 23
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  14 //total amount of sensors
#define ACTNUM  0 //total amount of actuators
#define DEVNUM  10 //total amount of internal devices

#define ALWAYSACTIVE 1 //1 if the game is always active

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 101};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}; // 1 playButton, 2:7 valvButton, 8 resetButton, 9:14 hamButton
const int ACTUATORS[ACTNUM] = {};
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58, 59, 60}; // 51 servo, 52 playLight, 53 resetLight, 54:60 valvRelay
const int RESET = 100;
const int ACTIVE = 124;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?

//Used Pins
const int sensPins[SENNUM] = {19, 17, 15, 22, 20, 18, 16, 35, 25, 24, 26, 28, 30, 32}; // Capacitor
const int actPins[ACTNUM] = {}; // relay
const int devPins[DEVNUM] = {23, 21, 33, 0, 2, 4, 6, 8, 7} ;

int sensStatus[SENNUM] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int lightPin[SENNUM] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;
int sequence[SENNUM] = {0, 1, 1, 0, 0, 0 , 1, 1, 1, 0};      //the right sequence
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence


 
const int MAX_LEVEL = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 4;

int velocity = 500;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);

  pinMode(5, OUTPUT);

  digitalWrite(5, LOW);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
}

void loop()
{
//  Serial.print("A0: "); Serial.println(digitalRead(A0));
//  Serial.print("A1: "); Serial.println(digitalRead(A1));
//  Serial.print("A2: "); Serial.println(digitalRead(A2));
//  Serial.print("A3: "); Serial.println(digitalRead(A3));
//  delay(velocity);
  if (level == 4)
    generate_sequence();//generate a sequence;

  if (deButton(A4, LOW)) //If start button is pressed
  {
    show_sequence();    //show the sequence
    get_sequence();     //wait for your sequence
  }
}

void show_sequence()
{
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);

  for (int i = 0; i < level; i++)
  {
    digitalWrite(sequence[i], HIGH);
    delay(velocity);
    digitalWrite(sequence[i], LOW);
    delay(200);
  }
}

void get_sequence()
{
  int flag = 0; //this flag indicates if the sequence is correct

  for (int i = 0; i < level; i++)
  {
    flag = 0;
    while (flag == 0)
    {
      if (deButton(A3, HIGH))
      {
        digitalWrite(6, HIGH);
        your_sequence[i] = 5;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(6, LOW);
      }

      if (deButton(A2, HIGH))
      {
        digitalWrite(4, HIGH);
        your_sequence[i] = 4;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(4, LOW);
      }

      if (deButton(A1, HIGH))
      {
        digitalWrite(3, HIGH);
        your_sequence[i] = 3;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(3, LOW);
      }

      if (deButton(A0, HIGH))
      {
        digitalWrite(2, HIGH);
        your_sequence[i] = 2;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(2, LOW);
      }

    }
  }
  right_sequence();
}

void generate_sequence()
{
  randomSeed(millis()); //in this way is really random!!!

  for (int i = 0; i < MAX_LEVEL; i++)
  {
    sequence[i] = random(2, 5);
  }
}
void wrong_sequence()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(6, HIGH);
    delay(250);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(6, LOW);
    delay(250);
  }
  level = 4;
}

void right_sequence()
{
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
  delay(250);

  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
  delay(500);

//  if (level < MAX_LEVEL);
//  level++;
 digitalWrite(5, HIGH);

}

boolean deButton (int pin, boolean state)
{
  boolean stateNow = digitalRead(pin);
  if(state==stateNow) 
  {
    delay(10);
    return HIGH;
  } 
  else return LOW;
}

