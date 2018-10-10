#include <Bounce.h>
#include <SPI.h>
#include <Ethernet.h>
#include "CCModBus.h"

CCModBus CCMod;                        //This creates a new instance of the CCModBus class.  The local instance is called CCMod.

byte MyMac[] = {0x04, 0xE9, 0xE5, 0x02, 0xB9, 0x25};
IPAddress MyIP(192, 168, 1, 11);                           //This needs to be unique in your network - only one puzzle can have this IP
IPAddress gateway(192, 168, 1, 254);                //This should be changed to be the gateway of your network
IPAddress subnet(255, 255, 255, 0);

IPAddress ClueControlIP (192, 168, 1, 100);                  //change this to the IP of your ClueControl Computer
boolean Pin5Hold;
boolean Pin6Hold;

const int LEVEL = 9;

boolean pressed = 0;
boolean solved = 0;

int sequence[LEVEL] = {0, 1, 1, 0, 0, 0 , 0, 0, 1};      //the right sequence
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence

boolean light[LEVEL] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

int switchPin[LEVEL] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
int lightPin[LEVEL] = {14, 15, 16, 17, 18, 19, 20, 21, 22} ;

int relayPin = A14;
int buzzerPin = 23;

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
//Bounce button9 = Bounce(switchPin[9], 10);

unsigned long interrupt_time = 0;

void setup() {

  Serial.begin(9600);
  Serial.println("Serial interface started");  //Serial interface is for debugging/demo

  Ethernet.begin(MyMac, MyIP, gateway, subnet);
  Serial.println("Ethernet interface started");

  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  CCMod.ClueControlIP = ClueControlIP;

  // print the ClueControl Address:
  Serial.print("ClueControl IP address: ");
  Serial.println(CCMod.ClueControlIP);
  Serial.println();


  pinMode(lightPin[0], OUTPUT);
  pinMode(lightPin[1], OUTPUT);
  pinMode(lightPin[2], OUTPUT);
  pinMode(lightPin[3], OUTPUT);
  pinMode(lightPin[4], OUTPUT);
  pinMode(lightPin[5], OUTPUT);
  pinMode(lightPin[6], OUTPUT);
  pinMode(lightPin[7], OUTPUT);
  pinMode(lightPin[8], OUTPUT);
  //  pinMode(lightPin[9], OUTPUT);

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
  //  pinMode(switchPin[9], INPUT_PULLUP);

  digitalWrite(lightPin[0], LOW);
  digitalWrite(lightPin[1], LOW);
  digitalWrite(lightPin[2], LOW);
  digitalWrite(lightPin[3], LOW);
  digitalWrite(lightPin[4], LOW);
  digitalWrite(lightPin[5], LOW);
  digitalWrite(lightPin[6], LOW);
  digitalWrite(lightPin[7], LOW);
  digitalWrite(lightPin[8], LOW);
  //  digitalWrite(lightPin[9], LOW);

  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  analogWrite(relayPin, 255);
  analogWrite(buzzerPin, 0);

  interrupt_time = 0;

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
  //  button9.update();

  // Check each button for "rising" edge
  // Type a message on the Keyboard when each button releases.
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  //          to high (not pressed - voltage from pullup resistor)
  if (button0.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(0);
    seq_cmp(yourSequence, sequence);
  }
  if (button1.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(1);
    seq_cmp(yourSequence, sequence);
  }
  if (button2.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(2);
    seq_cmp(yourSequence, sequence);
  }
  if (button3.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(3);
    seq_cmp(yourSequence, sequence);
  }
  if (button4.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(4);
    seq_cmp(yourSequence, sequence);
  }
  if (button5.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(5);
    seq_cmp(yourSequence, sequence);
  }
  if (button6.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(6);
    seq_cmp(yourSequence, sequence);
  }
  if (button7.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(7);
    seq_cmp(yourSequence, sequence);
  }
  if (button8.risingEdge()) {
    interrupt_time = millis();
    lightSwitch(8);
    seq_cmp(yourSequence, sequence);
  }

  if (pressed) {
    if (!seq_cmp2(yourSequence, sequence)) {
      if (millis() - interrupt_time > 2000) {
        for (int t = 0; t < 3; t++) {
          buzzer();
        }
        seq_clear(yourSequence);
        interrupt_time = millis();

      }
    }
  }
  //Check to see if a new command has been received by looking at the
  //CCMod.CCValue.
  if (CCMod.CCValue == 12)         //Could also use a switch statement instead of the IF statements
  {
    digitalWrite(6, HIGH);        //if command 12 is received, turn the LED on
    CCMod.CCValue = 0;        //very important to set the value back to 0, otherwise the IF statement
    //will fire again
    Serial.println("Message received from ClueControl - 12");
  }

  if (CCMod.CCValue == 13)
  {
    digitalWrite(6, LOW);         //if command 13 is received, turn the LED off
    CCMod.CCValue = 0;
  }

  //Check pin 5, send to CC if it goes low.  Once it goes low, block it from
  //sending again until the pin first goes high
  if (Pin5Hold)
  {
    if (digitalRead(5) == HIGH)
    {
      Serial.println("Pin 5 deactivated, sending 6-false");
      Pin5Hold = false;               //Set the hold flag so this IF doesn't trigger again and again
      CCMod.SetTrigger(6, false);    //This one command is all that is needed to send a value to ClueControl.
      //in this case, we send an update to make Trigger #20 false.  Note that it doesn'table
      //matter which puzzle the command comes from.  that means you could have another puzzle
      //that also changes the value of Trigger #20.  It is up to you if you want do to that or not.
    }
  }
  else
  {
    if (digitalRead(5) == LOW)
    {
      Serial.println("Pin 5 activated, sending 6-true");
      CCMod.SetTrigger(6, true);   //when pin5 is pulled low, set coil to true in ClueControl
      Pin5Hold = true;
    }

  }


  CCMod.ReceiveModBus();      //It is VERY important to call this in your main routine
  //this services the receiver of the ModBus.  It is during this routine that
  //the CCMod.CCValue will be updated if there is a new command from ClueControl
}

//compare the two sequences
boolean seq_cmp2(int *a, int *b) {
  for (int n = 0; n < LEVEL; n++)  if (a[n] != b[n]) return false;
  return true;
}

//clear the sequence
void seq_clear(int *s) {
  for (int n = 0; n < LEVEL; n++)  {
    s[n] = 0;
    digitalWrite(lightPin[n], 0);
    pressed = 0;
  }
}

//check the lights
void lightSwitch(int n) {
  light[n] = HIGH;
  yourSequence[n] = light[n];
  digitalWrite(lightPin[n], light[n]);
  pressed = 1;
  buzzer();
}

//compare the two sequences
void seq_cmp(int *a, int *b) {
  boolean equal = 1;
  for (int n = 0; n < LEVEL; n++) {
    if (a[n] != b[n]) {
      equal = 0;
    }
    if (equal) {
      analogWrite(relayPin, 250);
      solved = 1;
    }
    else {
      Serial.println("OK");
      analogWrite(relayPin, 0);
      solved = 0;
    }
  }
}

void buzzer() {
  analogWrite(buzzerPin, 200);
  delay(250);
  analogWrite(buzzerPin, 0);
  delay(50);
}


