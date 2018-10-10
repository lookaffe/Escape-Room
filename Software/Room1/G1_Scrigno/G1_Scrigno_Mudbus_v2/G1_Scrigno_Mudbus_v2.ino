//Teensy 3.5

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  10 //total amount of sensors
#define ACTNUM  1 //total amount of actuators
#define DEVNUM  10 //total amount of internal devices

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5}; //Dipende da ogni DEVICESitivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 101};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const int ACTUATORS[ACTNUM] = {101};
const int DEVICES[DEVNUM] = {51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
const int RESET = 100;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?

//Used Pins
const int sensPins[SENNUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 24}; // Capacitor
const int actPins[ACTNUM] = {39}; // relay
const int devPins[DEVNUM] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;

int sensStatus[SENNUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int sequence[SENNUM] = {0, 1, 1, 0, 0, 0 , 1, 1, 1, 0};      //the right sequence
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

/*
    Create Bounce objects for each button.  The Bounce object
    automatically deals with contact chatter or "bounce", and
    it makes detecting changes very simple.
*/
Bounce button0 = Bounce(sensPins[0], 100);
Bounce button1 = Bounce(sensPins[1], 100);  // 10 = 10 ms debounce time
Bounce button2 = Bounce(sensPins[2], 100);  // which is appropriate for
Bounce button3 = Bounce(sensPins[3], 100);  // most mechanical pushbuttons
Bounce button4 = Bounce(sensPins[4], 100);
Bounce button5 = Bounce(sensPins[5], 100);  // if a button is too "sensitive"
Bounce button6 = Bounce(sensPins[6], 100);  // to rapid touch, you can
Bounce button7 = Bounce(sensPins[7], 100);  // increase this time.
Bounce button8 = Bounce(sensPins[8], 100);
Bounce button9 = Bounce(sensPins[9], 100);

//ModbusIP object
Mudbus Mb;

void setup() {
  // reset for Ethernet Shield
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  delay(1000);
  digitalWrite(9, HIGH); // release the WIZ820io

  Ethernet.begin(mac, ip);
  delay(5000);

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = false;

  //Set light Pin mode
  for (int i = 0; i < DEVNUM ; i++) {
    pinMode(devPins[i], OUTPUT);
    digitalWrite(devPins[i], LOW);
  }
  
  // Configure the pins for input mode with pullup resistors.
  for (int i = 0; i < SENNUM ; i++) {
  pinMode(sensPins[i], INPUT);
  }

  pinMode(actPins[0], OUTPUT);
  digitalWrite(actPins[0], HIGH); // Open on LOW

}

void loop() {
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  /*
      Update all the buttons.  There should not be any long
      delays in loop(), so this runs repetitively at a rate
      faster than the buttons could be pressed and released.
  */
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();
  button8.update();
  button9.update();

  /*
     Check each button for "rising" edge
     rising = low (pressed - button connects pin to ground)
     to high (not pressed - voltage from pullup resistor)
  */
  if (button0.fallingEdge()) {
    fallingEdgeAction(0);
  }
  if (button1.fallingEdge()) {
    fallingEdgeAction(1);
  }
  if (button2.fallingEdge()) {
    fallingEdgeAction(2);
  }
  if (button3.fallingEdge()) {
    fallingEdgeAction(3);
  }
  if (button4.fallingEdge()) {
    fallingEdgeAction(4);
  }
  if (button5.fallingEdge()) {
    fallingEdgeAction(5);
  }
  if (button6.fallingEdge()) {
    fallingEdgeAction(6);
  }
  if (button7.fallingEdge()) {
    fallingEdgeAction(7);
  }
  if (button8.fallingEdge()) {
    fallingEdgeAction(8);
  }
  if (button9.fallingEdge()) {
    fallingEdgeAction(9);
  }
}

void fallingEdgeAction(int b) {
  if (sensStatus[b] == LOW) sensStatus[b] = HIGH;
  else sensStatus[b] = LOW;
  yourSequence[b] = sensStatus[b];
  digitalWrite(devPins[b], sensStatus[b]);
  Mb.R[SENSORS[b]] = sensStatus[b];
  Mb.R[DEVICES[b]] = sensStatus[b];

}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < SENNUM; n++) if (a[n] != b[n]) return false;
  return true;
}

void isPuzzleSolved() {
  puzzleSolved = (seq_cmp(yourSequence, sequence)) ? true : false;
  Mb.R[STATE] = puzzleSolved;
  trigger(actPins[0], puzzleSolved);
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  digitalWrite(s, !trig);
  delay(10);
}

// Resetta il gioco
void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], LOW);
  }
  triggered = false;
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    Mb.R[SENSORS[i]] = sensStatus[i];

    // reset specifico per il gioco
    yourSequence[i] = LOW;
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], LOW);
    Mb.R[DEVICES[i]] = LOW;
  }
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;
}

void listenFromEth() {
  if (Mb.R[RESET]) reset();
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = Mb.R[SENSORS[i]];
  }
  triggered = 0;
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(actPins[i], Mb.R[ACTUATORS[i]]);
    triggered = triggered || Mb.R[ACTUATORS[i]];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
  }
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  for (int i = 0; i < DEVNUM; i++) {
    Serial.print("DEVNUM "); Serial.print(i); Serial.print(" (reg "); Serial.print(DEVICES[i]); Serial.print(") - val:  "); Serial.println(Mb.R[DEVICES[i]]);
  }
  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  Serial.println();
  Serial.print("triggered: "); Serial.println(triggered);
  Serial.println();
}
