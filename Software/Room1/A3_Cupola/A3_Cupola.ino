int sensorPin = A3;
int sensorValue = 0;
boolean on = 0;
boolean dir = 0;
void setup() {
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  on = !on;
  digitalWrite(7, on);
  Serial.print("On: "); Serial.println(on);
  if (on) {
    for (int y = 0; y < 5; y++) {
      dir = !dir;
      Serial.print("Dir: "); Serial.println(dir);
      digitalWrite(8, dir);
      for (int i = 0; i < 1000; i++) {
        sensorValue = analogRead(sensorPin);
        sensorValue = map(sensorValue, 0 , 1023, 4000, 1000);
        digitalWrite(9, HIGH);
        delayMicroseconds(sensorValue);
        digitalWrite(9, LOW);
        delayMicroseconds(sensorValue);
      }
    }
  } else delay(4000);
}
