const int LEVEL = 10;
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

