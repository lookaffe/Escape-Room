#define SENNUM 6

unsigned long passedtime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long prevRotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
unsigned long rotationTime[SENNUM] = {0, 0, 0, 0, 0, 0};
boolean printTime = true;

void setup() {
  Serial.begin(9600);
  delay(1500);
  //Intiates Serial communications

  attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
  attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
  attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
  attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
  attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
  attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);

  pinMode(0, INPUT_PULLUP); //Sets hallsensor as input
  pinMode(1, INPUT_PULLUP); //Sets hallsensor as input
  pinMode(2, INPUT_PULLUP); //Sets hallsensor as input
  pinMode(3, INPUT_PULLUP); //Sets hallsensor as input
  pinMode(4, INPUT_PULLUP); //Sets hallsensor as input
  pinMode(5, INPUT_PULLUP); //Sets hallsensor as input
  for (int i = 0; i < SENNUM; i++) {
    passedtime[i] = millis();
    prevRotationTime[i] = millis();
  }

  Serial.println("FFF");
}

void loop() {
  if (printTime) {
    for (int i = 0; i < SENNUM; i++) {
      passedtime[i] = millis();
    }
    detachInterrupt(0);
    attachInterrupt(digitalPinToInterrupt(0), isr_0, RISING);
    detachInterrupt(1);
    attachInterrupt(digitalPinToInterrupt(1), isr_1, RISING);
    detachInterrupt(2);
    attachInterrupt(digitalPinToInterrupt(2), isr_2, RISING);
    detachInterrupt(3);
    attachInterrupt(digitalPinToInterrupt(3), isr_3, RISING);
    detachInterrupt(4);
    attachInterrupt(digitalPinToInterrupt(4), isr_4, RISING);
    detachInterrupt(5);
    attachInterrupt(digitalPinToInterrupt(5), isr_5, RISING);

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
  Serial.print("Rotation Time=");
  Serial.print(rotationTime[0]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[0]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[0] = millis();
  printTime = true;
}

void isr_1()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[1] = millis() - prevRotationTime[1];
  Serial.print("Rotation Time=");
  Serial.print(rotationTime[1]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[1]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[1] = millis();
  printTime = true;
}

void isr_2()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[2] = millis() - prevRotationTime[2];
  Serial.print("Rotation Time=");
  Serial.print(rotationTime[2]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[2]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[2] = millis();
  printTime = true;
}

void isr_3()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[3] = millis() - prevRotationTime[3];

  Serial.print("Rotation Time=");
  Serial.print(rotationTime[3]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[3]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[3] = millis();
  printTime = true;
}

void isr_4()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[4] = millis() - prevRotationTime[4];
  Serial.print("Rotation Time=");
  Serial.print(rotationTime[4]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[4]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[4] = millis();
  printTime = true;
}

void isr_5()
{
  //Each rotation, this interrupt function is run twice, so take that into consideration for
  //calculating RPM
  //Update count
  rotationTime[5] = millis() - prevRotationTime[5];
  Serial.print("Rotation Time=");
  Serial.print(rotationTime[5]); //Print out result to monitor
  Serial.print(" - Prev Rotation Time=");
  Serial.print(prevRotationTime[5]); //Print out result to monitor
  Serial.print(" - Millis=");
  Serial.println(millis()); //Print out result to monitor
  prevRotationTime[5] = millis();
  printTime = true;
}
