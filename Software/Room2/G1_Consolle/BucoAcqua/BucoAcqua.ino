//Teensy LC

const int waterPin = 21; //Pin Reed
const int relayPin = 0;
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
 Serial.println(analogRead(waterPin))  ;
}


