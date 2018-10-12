#define VALVNUM 6
const int valvPin[VALVNUM] = {0, 2, 4, 6, 8, 7};
const int buttonPin[VALVNUM] = {17, 15, 22, 20, 18, 16};
int buttonState[VALVNUM] = {0, 0, 0, 0, 0, 0};
const long fluxTime = 5000;


void setup() {
  Serial.begin(9600);
  for (int i = 0; i < VALVNUM; i++) {
    pinMode(valvPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
    digitalWrite(valvPin[i], LOW);  // LOW se uso SSRelay
  }
}
void loop() {

  for (int i = 0; i < 1; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);

    if (!buttonState[i]) {
      Serial.print(" Button "); Serial.print(i + 1); Serial.println(" Pressed");
      openValv(valvPin[i], fluxTime);
    }
  }
}

void openValv(int v, long t) {
  digitalWrite(v, HIGH); // HIGH se uso SSRelay
  Serial.println("Open");
  delay(t);
  digitalWrite(v, LOW);
  Serial.println("Closed");
}

