/* Buttons to USB Keyboard Example

   You must select Keyboard from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <Bounce.h>

const int LEVEL = 10;

int sequence[LEVEL] = {0, 1, 1, 0, 0, 0 , 1, 1, 1, 0};      //the right sequence
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

boolean light[LEVEL] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int switchPin[LEVEL] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int lightPin[LEVEL] = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23} ;

int relayPin = A14;

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
Bounce button0 = Bounce(switchPin[0], 10);
Bounce button1 = Bounce(switchPin[1], 10);  // 10 = 10 ms debounce time
Bounce button2 = Bounce(switchPin[2], 10);  // which is appropriate for
Bounce button3 = Bounce(switchPin[3], 10);  // most mechanical pushbuttons
Bounce button4 = Bounce(switchPin[4], 10);
Bounce button5 = Bounce(switchPin[5], 10);  // if a button is too "sensitive"
Bounce button6 = Bounce(switchPin[6], 10);  // to rapid touch, you can
Bounce button7 = Bounce(switchPin[7], 10);  // increase this time.
Bounce button8 = Bounce(switchPin[8], 10);
Bounce button9 = Bounce(switchPin[9], 10);

void setup() {

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

  // Configure the pins for input mode with pullup resistors.
  pinMode(switchPin[0], INPUT_PULLUP);
  pinMode(switchPin[1], INPUT_PULLUP);
  pinMode(switchPin[2], INPUT_PULLUP);
  pinMode(switchPin[3], INPUT_PULLUP);
  pinMode(switchPin[4], INPUT_PULLUP);
  pinMode(switchPin[5], INPUT_PULLUP);
  pinMode(switchPin[6], INPUT_PULLUP);
  pinMode(switchPin[7], INPUT_PULLUP);
  pinMode(switchPin[8], INPUT_PULLUP);
  pinMode(switchPin[9], INPUT_PULLUP);

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

  pinMode(relayPin, OUTPUT);

  analogWrite(relayPin, 255);
}

void loop() {
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
  button9.update();

  // Check each button for "rising" edge
  // Type a message on the Keyboard when each button releases.
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  if (button0.risingEdge()) {
    if (light[0] == LOW) light[0] = HIGH;
    else light[0] = LOW;
    yourSequence[0] = light[0];
    digitalWrite(lightPin[0], light[0]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button1.risingEdge()) {
    if (light[1] == LOW) light[1] = HIGH;
    else light[1] = LOW;
    yourSequence[1] = light[1];
    digitalWrite(lightPin[1], light[1]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button2.risingEdge()) {
    if (light[2] == LOW) light[2] = HIGH;
    else light[2] = LOW;
    yourSequence[2] = light[2];
    digitalWrite(lightPin[2], light[2]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button3.risingEdge()) {
    if (light[3] == LOW) light[3] = HIGH;
    else light[3] = LOW;
    yourSequence[3] = light[3];
    digitalWrite(lightPin[3], light[3]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button4.risingEdge()) {
    if (light[4] == LOW) light[4] = HIGH;
    else light[4] = LOW;
    yourSequence[4] = light[4];
    digitalWrite(lightPin[4], light[4]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button5.risingEdge()) {
    if (light[5] == LOW) light[5] = HIGH;
    else light[5] = LOW;
    yourSequence[5] = light[5];
    digitalWrite(lightPin[5], light[5]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button6.risingEdge()) {
    if (light[6] == LOW) light[6] = HIGH;
    else light[6] = LOW;
    yourSequence[6] = light[6];
    digitalWrite(lightPin[6], light[6]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button7.risingEdge()) {
    if (light[7] == LOW) light[7] = HIGH;
    else light[7] = LOW;
    yourSequence[7] = light[7];
    digitalWrite(lightPin[7], light[7]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button8.risingEdge()) {
    if (light[8] == LOW) light[8] = HIGH;
    else light[8] = LOW;
    yourSequence[8] = light[8];
    digitalWrite(lightPin[8], light[8]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
  if (button9.risingEdge()) {
    if (light[9] == LOW) light[9] = HIGH;
    else light[9] = LOW;
    yourSequence[9] = light[9];
    digitalWrite(lightPin[9], light[9]);
    if (seq_cmp(yourSequence, sequence)) {
      Serial.println("OK");
      analogWrite(relayPin, 0);
    } else analogWrite(relayPin, 255);
  }
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < LEVEL; n++) if (a[n] != b[n]) return false;
  return true;
}
