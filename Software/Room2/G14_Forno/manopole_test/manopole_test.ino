int tempPin = A5;    // select the input pin for the potentiometer
int timePin = A1;
int tempValue = 0;  // variable to store the value coming from the sensor
int timeValue = 0;

void setup() {
 
}

void loop() {
  // read the value from the sensor:
  tempValue = map(analogRead(tempPin),1,1023, 0, 250);    
  // turn the ledPin on
  timeValue = map(analogRead(timePin),1,1023, 0, 60);
  Serial.print("tempValue " + (String)tempValue + " - timeValue " + (String) timeValue);
  if(tempValue<215 && tempValue>205) Serial.print(" -- TEMP OK");
  if(timeValue<47 && timeValue>43) Serial.print(" -- TIME OK");
  Serial.println();
  delay(100);                
}
