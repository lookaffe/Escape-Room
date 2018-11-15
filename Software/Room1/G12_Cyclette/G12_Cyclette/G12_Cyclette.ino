#define SENNUM 6

int hallsensor[SENNUM] = {0, 1, 2, 3, 4, 5};           // Hall sensor at pin 2
unsigned long passedtime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long prevRotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long rotationTime[SENNUM] {0, 0, 0, 0, 0, 0};
boolean printTime = true;

void setup() {
  Serial.begin(9600);
  //Intiates Serial communications
  for (int i = 0; i < SENNUM; i++) {
    attachInterrupt(hallsensor[i], isr, RISING); //Interrupts are called on Rise of Input
    pinMode(hallsensor, INPUT_PULLUP); //Sets hallsensor as input
    passedtime[i] = 0; //Initialise the values
    prevRotationTime[i] = millis();
  }

}
void loop()
{
  if (printTime) {
    for (int i = 0; i < SENNUM; i++) {
      detachInterrupt(hallsensor[i]); //Interrupts are disabled
      passedtime[i] = millis();
      Serial.print("Rotation Time=");
      Serial.print(rotationTime[i]); //Print out result to monitor
      Serial.print(" - Prev Rotation Time=");
      Serial.print(prevRotationTime[i]); //Print out result to monitor
      Serial.print(" - Millis=");
      Serial.println(millis()); //Print out result to monitor
      delay(250);
      attachInterrupt(hallsensor[i], isr(i), RISING);   //Restart the interrupt processing
      printTime = false;
    }
  }
}

void isr(int i)
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime = millis() - prevRotationTime;
  prevRotationTime = millis();
  printTime = true;
}
