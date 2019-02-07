#include <Servo.h>

#define t1 5000 //avanti normale
#define t2 1000 // pausa
#define t3 200  // passetto indietro
#define t4 100 // pausa
#define t5 200 // passetto avanti
#define t6 100 // pausa
#define t7 3000 // avanti normale
#define t8 1000 // pausa
#define t9 100 // ripetuti avanti indietro
#define t10 5000 // indietro veloce fino a preistoria

Servo myservo;  // create servo object to control a servo

int potpin = 23;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
  myservo.attach(22);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  Serial.println(val);
  myservo.write(90);
  if (val == 0) {
    myservo.write(80);
    delay(t1);
    myservo.write(90);
    delay(t2);
    myservo.write(120);
    delay(t3);
    myservo.write(90);
    delay(t4);
    myservo.write(70);
    delay(t5);
    myservo.write(90);
    delay(t6);
    myservo.write(80);
    delay(t7);
    myservo.write(90);
    delay(t8);
    for (int i = 0 ; i < 8; i++) {
      myservo.write(50);
      delay(t9);
      myservo.write(130);
      delay(t9);
    }
    myservo.write(90);
    delay(t8);
    myservo.write(180);
    delay(t10);
    myservo.write(90);
  }
  if(val == 180){
    myservo.write(80);
    delay(300);
  }
}
