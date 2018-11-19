#define SENNUM 6

unsigned long passedtime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long prevRotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long rotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
volatile boolean printTime = true;
volatile int cycle = 0;
boolean firstTime = true;

void setup() {
  Serial.begin(9600);
  delay(3000);
  //Intiates Serial communications

  for (int i = 0; i < SENNUM; i++) {
    //attachInterrupt(digitalPinToInterrupt(i), isr_0, RISING);
    pinMode(i, INPUT_PULLUP); //Sets hallsensor as input
    passedtime[i] = millis();
    prevRotationTime[i] = millis();
  }

  Serial.println("FFF");
}

void loop() {
  if (firstTime) first();
  for (int i = 0; i < SENNUM; i++) {
    detachInterrupt(i);
    (millis() - prevRotationTime[i] > 3000) ? rotationTime[i] = 0 : rotationTime[i];
    switch (i) {
      case 0:
        attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
        break;
      case 1:
        attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
        break;
      case 4:
        attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
        break;
      case 5:
        attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);
        break;
    }
  }


  if (printTime) {
    Serial.print("Cyclette: ");
    Serial.print(cycle); //Print out result to monitor
    Serial.print(" - RPM=");
    Serial.print(rotationTime[cycle]); //Print out result to monitor
    Serial.print(" - Prev Rot Time=");
    Serial.print(prevRotationTime[cycle]); //Print out result to monitor
    Serial.print(" - Millis=");
    Serial.println(millis()); //Print out result to monitor
    printTime = false;
    Serial.println();
  }
}

void isr_0()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[0] = millis() - prevRotationTime[0];
  prevRotationTime[0] = millis();
  cycle = 0;
  printTime = true;
}

void isr_1()
{
  rotationTime[1] = millis() - prevRotationTime[1];
  prevRotationTime[1] = millis();
  cycle = 1;
  printTime = true;
}

void isr_2()
{
  rotationTime[2] = millis() - prevRotationTime[2];
  prevRotationTime[2] = millis();
  cycle = 2;
  printTime = true;
}

void isr_3()
{
  rotationTime[3] = millis() - prevRotationTime[3];
  prevRotationTime[3] = millis();
  cycle = 3;
  printTime = true;
}

void isr_4()
{
  rotationTime[4] = millis() - prevRotationTime[4];
  prevRotationTime[4] = millis();
  cycle = 4;
  printTime = true;
}

void isr_5()
{
  rotationTime[5] = millis() - prevRotationTime[5];
  prevRotationTime[5] = millis();
  cycle = 5;
  printTime = true;
}

void first() {
  firstTime = false;
  attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
  attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
  attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
  attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
  attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
  attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);
  Serial.println("Interrupt all ");
}
