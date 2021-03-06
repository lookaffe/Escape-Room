//Teensy 3.2

#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Bounce.h>

#define SENNUM  9 //total amount of sensors
#define ACTNUM  1 //total amount of actuators

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0xE6, 0xE1, 26}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino
uint8_t ip[] = {10, 0, 0, 106};                           //This needs to be unique in your network - only one puzzle can have this IP

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int SENSORS[SENNUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
const int ACTUATORS[ACTNUM] = {101};
const int RESET = 999;

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?

//Used Pins
int sensPins[SENNUM] = {23, 21, 19, 17, 15, 22, 20, 18, 16}; // switch
int actPins[ACTNUM] = {4}; //buzzer

int sequence[SENNUM] = {0, 1, 1, 0, 0, 0 , 0, 0, 1};      //the right sequence
int yourSequence[SENNUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

boolean sensStatus[SENNUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

boolean pressed = false;

unsigned long interrupt_time = 0;

/*
    Create Bounce objects for each button.  The Bounce object
    automatically deals with contact chatter or "bounce", and
    it makes detecting changes very simple.
*/
Bounce button0 = Bounce(sensPins[0], 10);
Bounce button1 = Bounce(sensPins[1], 10);  // 10 = 10 ms debounce time
Bounce button2 = Bounce(sensPins[2], 10);  // which is appropriate for
Bounce button3 = Bounce(sensPins[3], 10);  // most mechanical pushbuttons
Bounce button4 = Bounce(sensPins[4], 10);
Bounce button5 = Bounce(sensPins[5], 10);  // if a button is too "sensitive"
Bounce button6 = Bounce(sensPins[6], 10);  // to rapid touch, you can
Bounce button7 = Bounce(sensPins[7], 10);  // increase this time.
Bounce button8 = Bounce(sensPins[8], 10);

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

  // Configure the pins for input mode with pullup resistors.
  pinMode(sensPins[0], INPUT);
  pinMode(sensPins[1], INPUT);
  pinMode(sensPins[2], INPUT);
  pinMode(sensPins[3], INPUT);
  pinMode(sensPins[4], INPUT);
  pinMode(sensPins[5], INPUT);
  pinMode(sensPins[6], INPUT);
  pinMode(sensPins[7], INPUT);
  pinMode(sensPins[8], INPUT);

  pinMode(actPins[0], OUTPUT);
  digitalWrite(actPins[0], LOW);

  interrupt_time = millis();

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
  // Update all the buttons.  There should not be any long
  // delays in loop(), so this runs repetitively at a rate
  // faster than the buttons could be pressed and released.
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();
  button8.update();

  // Check each button for "falling" edge
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
}

void fallingEdgeAction(int b) {
  pressed = true;
  buzzer(340, 300);
  sensStatus[b] = HIGH;
  yourSequence[b] = sensStatus[b];
  Mb.R[SENSORS[b]] = sensStatus[b];
  interrupt_time = millis();
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < SENNUM; n++)  if (a[n] != b[n]) return false;
  return true;
}

void isPuzzleSolved() {
  if (pressed) {
    if (millis() - interrupt_time > 3000) {
      pressed = 0;
      interrupt_time = millis();
      if (seq_cmp(yourSequence, sequence)) puzzleSolved = true;
      else {
        puzzleSolved = false;
        buzzer(220, 1000);
        seq_clear(yourSequence);
      }
      Mb.R[STATE] = puzzleSolved;
    }
  }
}

//clear the sequence
void seq_clear(int *s) {
  for (int n = 0; n < SENNUM; n++)  {
    s[n] = 0;
    sensStatus[n] = LOW;
    Mb.R[SENSORS[n]] = sensStatus[n];
    pressed = 0;
    interrupt_time = millis();
  }
}

void buzzer(unsigned int freq, unsigned long vel) {
  tone(actPins[0], freq, vel);
}

// Azione su ricezione comando "trigger"
void trigger(int s, boolean trig) {
  Mb.R[ACTUATORS[s]] = trig;
  if (trig) buzzer(500, 1000);
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
  }
  // reset specifico per il gioco
  seq_clear(yourSequence);
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
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
    Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
    Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  Serial.print("RESET (reg "); Serial.print(RESET); Serial.print(") - val:  "); Serial.println(Mb.R[RESET]);
  Serial.println();
  Serial.print("triggered: "); Serial.println(triggered);
  Serial.println();
}
