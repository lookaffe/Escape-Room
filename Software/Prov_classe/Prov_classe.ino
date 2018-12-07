//Teensy 3.5

#include "EscapeRoom.h"
#include <Bounce.h>

#define SENNUM 10
#define ACTNUM 1
#define DEVNUM 10

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 0, 101};                           //This needs to be unique in your network - only one puzzle can have this IP

//Used Pins
uint8_t sensPins[SENNUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 24}; // Capacitor
uint8_t actPins[ACTNUM] = {39}; // relay
uint8_t devPins[DEVNUM] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;

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

EscapeRoom er = EscapeRoom(SENNUM, ACTNUM, DEVNUM, 1);

void setup() {
  er.setupEscape(mac, ip);
  er.setPins(sensPins, actPins, devPins);
}

void loop() {
  er.listenFromEth();
  if (!er._triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  er.printRegisters();
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
  er.writeSenReg(b, sensStatus[b]);
  er.writeDevReg(b, sensStatus[b]);
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < SENNUM; n++) if (a[n] != b[n]) return false;
  return true;
}

void isPuzzleSolved() {
  er._puzzleSolved = (seq_cmp(yourSequence, sequence)) ? true : false;
  er.writeStaReg(er._puzzleSolved);
  er.triggerAct(0, er._puzzleSolved);
}


void specificReset()  {
    // reset specifico per il gioco
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = LOW;
    yourSequence[i] = LOW;
  }
} override;

