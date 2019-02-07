/*
  Playing with the HC-SR04 Ultrasonic Sensor
  3/26/17 by Brian Patton
  Feel free to do whatever you want with this code example
*/
byte trigPin = 21;    // Trigger is connected to this pin
byte echoPin = 22;    // Echo is connected to this pin
float echoTime;       // Variable to store echo pulse time  
float CMs;            // Variable to carry Cm values
float Inches;         // Variable to carry Inch values
float distance;       // Variable to hold calculated distance 
//************************************************************
//                Setup
//************************************************************
void setup() {
  pinMode(trigPin, OUTPUT);   // Set pin to Output
  pinMode(echoPin, INPUT);    // Set pin to Intput
  digitalWrite(trigPin, LOW); // Start the tigger pin off as low
  Serial.begin(9600);         // Open the Serial Port
}
//************************************************************
//                Main Loop
//************************************************************
void loop() {
  distance = echoCM();    // Call non-void function to get the distance in cm
  Serial.println("Distance in Cms = " + (String)distance);    // print data
  distance = echoInches();// Call non-void function to get the distance in inches
  Serial.println("Distance in inches = " + (String)distance); // print data
  Serial.println(" ");    // Print blank Space
  delay(300);             // Delay 1/2 second
}
//************************************************************
//                Calculate distance in cm
//************************************************************
float echoCM() {
  digitalWrite(trigPin, LOW);  // Make sure the pin starts off low
  delay(1);                    // Min between triggers is 50us so let's do 1000us
  digitalWrite(trigPin, HIGH); // Set trigger pin High for....
  delayMicroseconds(10);       // 10 microseconds.
  digitalWrite(trigPin, LOW);  // Set it low again

  echoTime = pulseIn(echoPin, HIGH);  //Collect the echo pulse
  CMs = echoTime / 58;                //Conversion in Data Sheet
  return CMs;                         // Return the calculated value
}
//************************************************************
//                Calculate distance in inches
//************************************************************
float echoInches() {
  digitalWrite(trigPin, LOW);  // Make sure the pin starts off low
  delay(1);                    // Min between triggers is 50us so let's do 1000
  digitalWrite(trigPin, HIGH); // Set trigger pin High for....
  delayMicroseconds(10);       // 10 microseconds.
  digitalWrite(trigPin, LOW);  // Set it low again

  echoTime = pulseIn(echoPin, HIGH);  //Collect the echo pulse
  Inches = echoTime / 148;            //Conversion in Data Sheet
  return Inches;                      // Return the calculated value
}
