//Teensy LC

const int waterPin = 23; //Pin Reed
const int relayPin = 21;
boolean puzzleSolved = false;
void setup()
{
  pinMode(relayPin, OUTPUT); //Imposto i PIN
  pinMode(waterPin, INPUT);
  digitalWrite(relayPin, HIGH);
  Serial.begin(9600);
}
void loop()
{
 Serial.println(digitalRead(waterPin))  ;
}


