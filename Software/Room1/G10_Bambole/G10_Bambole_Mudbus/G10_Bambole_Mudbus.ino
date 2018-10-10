//
////Teensy LC
//
//#include <SPI.h>
//#include <Ethernet.h>
//#include <Mudbus.h>
//#include <Bounce.h>
//
//#define SENNUM  1 //total amount of sensors
//#define ACTNUM  0 //total amount of actuators
//
//uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x9A}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
//uint8_t ip[] = {10, 0, 0, 103};                       //This needs to be unique in your network - only one puzzle can have this IP
//
////Modbus Registers Offsets (0-9999)
//const int STATE = 0;
//const int SENSORS[SENNUM] = {1};
//const int ACTUATORS[ACTNUM] = {};
//const int RESET = 999;
//
//// Track the room game state
//bool puzzleSolved = false;  // has the puzzle in the room been solved?
//bool triggered = false; // has the control room triggered some actuator?
////Used Pins
//const int sensPins[SENNUM] = {16}; // switch
//Bounce button0 = Bounce(pinSwitch, 10); //Pin button
//int sensStatus[SENNUM] = {LOW};
//
////ModbusIP object
//Mudbus Mb;
//
//void setup()
//{
//  Serial.begin(9600);
//  // reset for Ethernet Shield
//  pinMode(9, OUTPUT);
//  digitalWrite(9, LOW); // reset the WIZ820io
//  delay(1000);
//  digitalWrite(9, HIGH); // release the WIZ820io
//
//  Ethernet.begin(mac, ip);
//  delay(5000);
//
//  Serial.print("server is at ");
//  Serial.println(Ethernet.localIP());
//
//  // Initial game state
//  puzzleSolved = false;
//
//  //Set Pin mode
//  pinMode(actPins[0], OUTPUT);
//  pinMode(sensPins[0], INPUT_PULLUP);
//  digitalWrite(actPins[0], LOW); //Open on HIGH
//}
//
//void loop()
//{
//  Mb.Run();
//  listenFromEth();
//  if (!triggered) {
//  gameUpdate();
//  isPuzzleSolved();
//  }
//  printRegister();
//}
//
//void gameUpdate() {
//  button0.update();
//
//  if (button0.fallingEdge()) puzzleSolved = true;
//  if (button0.risingEdge()) puzzleSolved = false;
//  Mb.R[STATE] = puzzleSolved;
//}
//
//void isPuzzleSolved() {
//}
//
//// Azione su ricezione comando "trigger"
//void trigger(int s, boolean trig) {
//  Mb.R[ACTUATORS[s]] = trig;
//  digitalWrite(s, trig);
//  delay(10);
//}
//
//// Resetta il gioco
//void reset() {
//  for (int i = 0; i < ACTNUM ; i++) {
//    trigger(actPins[i], LOW);
//  }
//  triggered = false;
//  for (int i = 0; i < SENNUM ; i++) {
//    sensStatus[i] = LOW;
//    Mb.R[SENSORS[i]] = sensStatus[i];
//  }
//  puzzleSolved = false;
//  Mb.R[STATE] = puzzleSolved;
//  Mb.R[RESET] = LOW;
//}
//
//void listenFromEth() {
//  if (Mb.R[RESET]) reset();
//  for (int i = 0; i < SENNUM ; i++) {
//    sensStatus[i] = Mb.R[SENSORS[i]];
//  }
//  triggered=0;
//  for (int i = 0; i < ACTNUM ; i++) {
//    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
//    triggered = triggered || Mb.R[ACTUATORS[i]];
//  }
//}
//
//void printRegister() {
//  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
//  for (int i = 0; i < SENNUM; i++) {
//    Serial.print("SENSORS ");Serial.print(i);Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
//  }
//  for (int i = 0; i < ACTNUM; i++) {
//    Serial.print("ACTUATORS ");Serial.print(i);Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
//  }
//  Serial.print("RESET (reg "); Serial.print(RESET);Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
//  Serial.println();
//  Serial.print("triggered: "); Serial.println(triggered);
//  Serial.println();
//}const int LEVEL = 10;
int sequence[LEVEL] = {0, 1, 1, 0, 0, 0 , 1, 1, 1, 0};      //the right sequence
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence
//int relayPin = 6;
boolean light[LEVEL] = {0,0,0,0,0,0,0,0,0,0};
//boolean light_02 = 0;
//boolean light_03 = 0;
//boolean light_04 = 0;
//boolean light_05 = 0;
//boolean light_06 = 0;
//boolean light_07 = 0;
//boolean light_08 = 0;
//boolean light_09 = 0;
//boolean light_10 = 0;

int switchPin[LEVEL] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//int switchPin_02 = 1;
//int switchPin_03 = 2;
//int switchPin_04 = 3;
//int switchPin_05 = 4;
//int switchPin_06 = 5;
//int switchPin_07 = 6;
//int switchPin_08 = 7;
//int switchPin_09 = 8;
//int switchPin_10 = 9;

int lightPin[LEVEL] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;
//int lightPin_02 = 15;
//int lightPin_03 = 16;
//int lightPin_04 = 17;
//int lightPin_05 = 18;
//int lightPin_06 = 19;
//int lightPin_07 = 20;
//int lightPin_08 = 21;
//int lightPin_09 = 22;
//int lightPin_10 = 23;
int i =0;

void setup() {
  Serial.begin(9600);
  pinMode(lightPin[0], OUTPUT);
  pinMode(lightPin[1], OUTPUT);
  pinMode(lightPin[2], OUTPUT);
  pinMode(lightPin[3], OUTPUT);
  pinMode(lightPin[4], OUTPUT);
  pinMode(lightPin[5], OUTPUT);
  pinMode(lightPin[6], OUTPUT);
  pinMode(lightPin[7], OUTPUT);
  pinMode(lightPin[8], OUTPUT);
  pinMode(lightPin[9], OUTPUT);

  pinMode(switchPin[0], INPUT);
  pinMode(switchPin[1], INPUT);
  pinMode(switchPin[2], INPUT);
  pinMode(switchPin[3], INPUT);
  pinMode(switchPin[4], INPUT);
  pinMode(switchPin[5], INPUT);
  pinMode(switchPin[6], INPUT);
  pinMode(switchPin[7], INPUT);
  pinMode(switchPin[8], INPUT);
  pinMode(switchPin[9], INPUT);

  digitalWrite(lightPin[0], LOW);
  digitalWrite(lightPin[1], LOW);
  digitalWrite(lightPin[2], LOW);
  digitalWrite(lightPin[3], LOW);
  digitalWrite(lightPin[4], LOW);
  digitalWrite(lightPin[5], LOW);
  digitalWrite(lightPin[6], LOW);
  digitalWrite(lightPin[7], LOW);
  digitalWrite(lightPin[8], LOW);
  digitalWrite(lightPin[9], LOW);
}

void loop()
{
  for (i = 0; i < LEVEL ; i++) deBounce(switchPin[i]);
}


void deBounce (int pin)
{
  boolean stateNow = digitalRead(pin);
  delay(20);
  if (stateNow) {
    if (light[pin] == LOW) light[pin] = HIGH;
    else light[pin] = LOW;
    yourSequence[pin] = light[pin];
    digitalWrite(lightPin[pin], light[pin]);
    if (seq_cmp(yourSequence, sequence)) Serial.println("OK"); //compare the two sequences
  }

}


//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < LEVEL; n++) if (a[n] != b[n]) return false;
  return true;
}

