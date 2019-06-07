#include <Servo.h>
 
Servo myservo;  // crea el objeto servo
int pos;    // posicion del servo

void setup() {
   myservo.attach(23);  // vincula el servo al pin digital 9
}
 
void loop() {
   //varia la posicion de 20 a 160, con esperas de 10ms
   for (pos = 20; pos <= 180; pos += 1) 
   {
      myservo.write(pos);              
      delay(25);                       
   }
 
 delay(500);
 
   //varia la posicion de 160 a 20, con esperas de 10ms
   for (pos = 180; pos >= 20; pos -= 1) 
   {
      myservo.write(pos);              
      delay(25);
                          
   }
   delay(500);
   
}
