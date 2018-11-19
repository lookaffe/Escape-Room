#define VALVNUM 1
const int valvPin[VALVNUM] = {0};//, 1, 2, 3, 4, 5};
const int buttonPin[VALVNUM] = {18};//, 19, 20, 21, 22, 23};
int buttonState[VALVNUM] = {0};//, 0, 0, 0, 0, 0};
const long fluxTime = 2900;


void setup() {
  Serial.begin(9600);
  pinMode(valvPin[0], OUTPUT);
  pinMode(buttonPin[0], INPUT);
  digitalWrite(valvPin[0], HIGH);

//  pinMode(valvPin[1], OUTPUT);
//  pinMode(buttonPin[1], INPUT);
//  digitalWrite(valvPin[1], HIGH);
//
//  pinMode(valvPin[2], OUTPUT);
//  pinMode(buttonPin[2], INPUT);
//  digitalWrite(valvPin[2], HIGH);
//
//  pinMode(valvPin[3], OUTPUT);
//  pinMode(buttonPin[3], INPUT);
//  digitalWrite(valvPin[3], HIGH);
//
//  pinMode(valvPin[4], OUTPUT);
//  pinMode(buttonPin[4], INPUT);
//  digitalWrite(valvPin[4], HIGH);
//
//  pinMode(valvPin[5], OUTPUT);
//  pinMode(buttonPin[5], INPUT);
//  digitalWrite(valvPin[5], HIGH);
}
void loop() {
  
  for (int i = 0; i < VALVNUM; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);
    
    if (buttonState[i]) {
      Serial.print(" Button ");Serial.print(i+1);Serial.println(" Pressed");
      openValv(valvPin[i], fluxTime);
    }
  }
}

void openValv(int v, long t) {
  digitalWrite(v, LOW);
  Serial.println("Open");
  delay(t);
  digitalWrite(v, HIGH);
  Serial.println("Closed");
}

