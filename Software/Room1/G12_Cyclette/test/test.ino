int hallsensor = 2;           // Hall sensor at pin 2
unsigned long passedtime = 0;
unsigned long prevRotationTime = 0;
unsigned long rotationTime = 0;
boolean printTime = true;
int count = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("PrinTime"); Serial.println(printTime);
  //Intiates Serial communications
  attachInterrupt(hallsensor, isr, RISING); //Interrupts are called on Rise of Input
  pinMode(hallsensor, INPUT_PULLUP); //Sets hallsensor as input
  passedtime = 0; //Initialise the values
  prevRotationTime = millis();
}

void loop() {
  if (printTime) {
    detachInterrupt(hallsensor); //Interrupts are disabled
    passedtime = millis();
    Serial.print("Rotation Time=");
    Serial.print(rotationTime); //Print out result to monitor
    Serial.print(" - Prev Rotation Time=");
    Serial.print(prevRotationTime); //Print out result to monitor
    Serial.print(" - Millis=");
    Serial.println(millis()); //Print out result to monitor
    delay(250);
    attachInterrupt(hallsensor, isr, RISING);   //Restart the interrupt processing
    printTime = false;
  }
}

void isr()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime = millis() - prevRotationTime;
  prevRotationTime = millis();
  printTime = true;
}
