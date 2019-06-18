#include <Servo.h>
 
Servo myservo;
int pos;

void setup() {
   myservo.attach(3);
   delay(2000);
}
 
void loop() {
   for (pos = myservo.read(); pos <= 180; pos += 1) 
   {
      myservo.write(pos);              
      delay(25);                       
   }
 
 delay(500);
 
   for (pos = 180; pos >= 20; pos -= 1) 
   {
      myservo.write(pos);              
      delay(25);
                          
   }
   delay(500);
   
}
