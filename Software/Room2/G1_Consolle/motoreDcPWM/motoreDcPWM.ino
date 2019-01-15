// Henry's Bench Small DC Motor

int motorDrive = 22;  // The output to the transistor that drives the motor
int timeOn = 2500;  //amount of time motor is on
int timeOff = 1000; //amount of time motor is off

void setup()
{
 pinMode(motorDrive, OUTPUT); 
}


void loop() // run over and over again
{
 analogWrite(motorDrive, 30); // Top Speed
 delay(timeOn); // number of milliseconds to keep on
 analogWrite(motorDrive, 128); // Lower Speed
 delay(timeOff); // number of milliseconds to keep off
}
