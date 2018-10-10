// TEENSY 3.2 or 3.5
const int LEVEL = 10;
int sequence[LEVEL] = {0, 1, 1, 0, 0, 0 ,1, 1, 1, 0};       //the right sequence
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //user sequence
int relayPin = A14;
boolean light_01 = 0;
boolean light_02 = 0;
boolean light_03 = 0;
boolean light_04 = 0;
boolean light_05 = 0;
boolean light_06 = 0;
boolean light_07 = 0;
boolean light_08 = 0;
boolean light_09 = 0;
boolean light_10 = 0;

int switchPin_01 = 0;
int switchPin_02 = 1;
int switchPin_03 = 2;
int switchPin_04 = 3;
int switchPin_05 = 4;
int switchPin_06 = 5;
int switchPin_07 = 6;
int switchPin_08 = 7;
int switchPin_09 = 8;
int switchPin_10 = 9;

int lightPin_01 = 14 ;
int lightPin_02 = 15;
int lightPin_03 = 16;
int lightPin_04 = 17;
int lightPin_05 = 18;
int lightPin_06 = 19;
int lightPin_07 = 20;
int lightPin_08 = 21;
int lightPin_09 = 22;
int lightPin_10 = 23;

void setup() {
  Serial.begin(9600);
  pinMode(lightPin_01, OUTPUT);
  pinMode(lightPin_02, OUTPUT);
  pinMode(lightPin_03, OUTPUT);
  pinMode(lightPin_04, OUTPUT);
  pinMode(lightPin_05, OUTPUT);
  pinMode(lightPin_06, OUTPUT);
  pinMode(lightPin_07, OUTPUT);
  pinMode(lightPin_08, OUTPUT);
  pinMode(lightPin_09, OUTPUT);
  pinMode(lightPin_10, OUTPUT);

  pinMode(relayPin, OUTPUT);

  pinMode(switchPin_01, INPUT);
  pinMode(switchPin_02, INPUT);
  pinMode(switchPin_03, INPUT);
  pinMode(switchPin_04, INPUT);
  pinMode(switchPin_05, INPUT);
  pinMode(switchPin_06, INPUT);
  pinMode(switchPin_07, INPUT);
  pinMode(switchPin_08, INPUT);
  pinMode(switchPin_09, INPUT);
  pinMode(switchPin_10, INPUT);

//  pinMode(relayPin, OUTPUT);
//
//  digitalWrite(relayPin, LOW);

  digitalWrite(lightPin_01, LOW);
  digitalWrite(lightPin_02, LOW);
  digitalWrite(lightPin_03, LOW);
  digitalWrite(lightPin_04, LOW);
  digitalWrite(lightPin_05, LOW);
  digitalWrite(lightPin_06, LOW);
  digitalWrite(lightPin_07, LOW);
  digitalWrite(lightPin_08, LOW);
  digitalWrite(lightPin_09, LOW);
  digitalWrite(lightPin_10, LOW);

analogWrite(relayPin, 255);


  attachInterrupt(switchPin_01, lightOn01, RISING);
  attachInterrupt(switchPin_02, lightOn02, RISING);
  attachInterrupt(switchPin_03, lightOn03, RISING);
  attachInterrupt(switchPin_04, lightOn04, RISING);
  attachInterrupt(switchPin_05, lightOn05, RISING);
  attachInterrupt(switchPin_06, lightOn06, RISING);
  attachInterrupt(switchPin_07, lightOn07, RISING);
  attachInterrupt(switchPin_08, lightOn08, RISING);
  attachInterrupt(switchPin_09, lightOn09, RISING);
  attachInterrupt(switchPin_10, lightOn10, RISING);
}

void loop()
{

  if (seq_cmp(yourSequence, sequence)) {
    Serial.println("OK"); //compare the two sequences
    analogWrite(relayPin, 0);
  } else analogWrite(relayPin, 255);

}

void lightOn01() { //if the button is pressed change the state of the LED and change the value in the yourSequence array
  Serial.println("1 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_01 == LOW) light_01 = HIGH;
    else light_01 = LOW;
    yourSequence[0] = light_01;
    digitalWrite(lightPin_01, light_01);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn02() {
  Serial.println("2 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_02 == LOW) light_02 = HIGH;
    else light_02 = LOW;
    yourSequence[1] = light_02;
    digitalWrite(lightPin_02, light_02);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn03() {
  Serial.println("3 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_03 == LOW) light_03 = HIGH;
    else light_03 = LOW;
    yourSequence[2] = light_03;
    digitalWrite(lightPin_03, light_03);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn04() {
  Serial.println("4 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_04 == LOW) light_04 = HIGH;
    else light_04 = LOW;
    yourSequence[3] = light_04;
    digitalWrite(lightPin_04, light_04);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn05() {
  Serial.println("5 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_05 == LOW) light_05 = HIGH;
    else light_05 = LOW;
    yourSequence[4] = light_05;
    digitalWrite(lightPin_05, light_05);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn06() {
  Serial.println("6 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_06 == LOW) light_06 = HIGH;
    else light_06 = LOW;
    yourSequence[5] = light_06;
    digitalWrite(lightPin_06, light_06);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn07() {
  Serial.println("7 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_07 == LOW) light_07 = HIGH;
    else light_07 = LOW;
    yourSequence[6] = light_07;
    digitalWrite(lightPin_07, light_07);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn08() {
  Serial.println("8 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_08 == LOW) light_08 = HIGH;
    else light_08 = LOW;
    yourSequence[7] = light_08;
    digitalWrite(lightPin_08, light_08);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn09() {
  Serial.println("9 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_09 == LOW) light_09 = HIGH;
    else light_09 = LOW;
    yourSequence[8] = light_09;
    digitalWrite(lightPin_09, light_09);
  }
  last_interrupt_time = interrupt_time;
}

void lightOn10() {
  Serial.println("10 pressed");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (light_10 == LOW) light_10 = HIGH;
    else light_10 = LOW;
    yourSequence[9] = light_10;
    digitalWrite(lightPin_10, light_10);
  }
  last_interrupt_time = interrupt_time;
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) { 
  for (int n = 0; n < LEVEL; n++) if (a[n] != b[n]) return false;
  return true;
}

