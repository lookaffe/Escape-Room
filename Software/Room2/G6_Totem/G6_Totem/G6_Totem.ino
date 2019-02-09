//Teensy 3.2

//#define ONLINE

#define SENNUM  2       //total amount of sensors
#define ACTNUM  1       //total amount of actuators
#define DEVNUM  2       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

const int senPins[SENNUM] = {21, 23}; // Ultrasuono alto, Ultrasuono basso
const int actPins[ACTNUM] = {2}; // elettrovalvola nuvola
const int devPins[DEVNUM] = {0, 1}; // luce alta, luce bassa

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 106};                     //This needs to be unique in your network - only one puzzle can have this IP

int stato = 0;

void resetSpec() {
  stato = 0;
}

byte trigPin[2] = {20, 22};  // Trigger is connected to this pin
byte echoPin[2] = {21, 23};   // Echo is connected to this pin
float echoTime;       // Variable to store echo pulse time
float CMs;            // Variable to carry Cm values

#include <EscapeFunction.h>

const float distance = 70.00;

elapsedMillis lapUp, lapDown = 0;
const long lapTime = 3000;
elapsedMillis danceUp, danceDown = 0;
const long danceTime = 10000;
int count = 0;

bool is = LOW;

void setup()
{
  setupEscape();
  pinMode(trigPin[0], OUTPUT);   // Set pin to Output
  pinMode(trigPin[1], OUTPUT);   // Set pin to Output
  pinMode(echoPin[0], INPUT);    // Set pin to Intput
  pinMode(echoPin[1], INPUT);    // Set pin to Intput
  digitalWrite(trigPin[0], LOW); // Start the tigger pin off as low
  digitalWrite(trigPin[1], LOW); // Start the tigger pin off as low
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
  Serial.println(); Serial.println(); Serial.println(); Serial.println(); Serial.println(); Serial.println();
}

void gameUpdate() {
  float dist[2] = {0.0, 0.0};
  for (int i = 0; i < 2; i++) {
    dist[i] = echoCM(i);    // Call non-void function to get the distance in cm
    myDelay(100);             // Delay 1/2 second
  }
  Serial.println("lapUp " + (String)lapUp);
  switch (stato) {
    case 0:
      stato = Mb.R[ACTIVE];
      lapUp = 0;
      danceUp = 0;
      break;
    case 1:
      if (dist[0] < distance) { // 1. se passo davanti al sensore alto
        if (lapUp < lapTime) { // 3. se il tempo del giro è minore di quello stabilito (non sei lento)
          if (danceUp > danceTime) {           
            stato = 2; // 4. Se il tempo di danza è maggiore di quello stabilito (hai ballato abbastanza)
            lapDown = 0; 
            danceDown = 0;
          }
        } else {
          danceUp = 0; // 5. altrimenti aggiorno il tempo di ballo
        }
        lapUp = 0; // 2. comunque aggiorno il tempo del giro
      }
      sensorRegUpdate(0, stato);
      break;
    case 2:
      if (dist[0] < distance && lapUp > 1500) stato = 1; // 0. se passo davanti al sensore alto, ritorno allo stato 1
      if (dist[1] < distance) { // 1. se passo davanti al sensore basso
        if (lapDown < lapTime) { // 3. se il tempo del giro è minore di quello stabilito (non sei lento)
          if (danceDown > danceTime) {
            puzzleSolved = 1;  // 4. Se il tempo di danza è maggiore di quello stabilito (hai ballato abbastanza)
          }
        } else {
          danceDown = 0; // 5. altrimenti aggiorno il tempo di ballo
        }
        lapDown = 0; // 2. comunque aggiorno il tempo del giro
      }
      sensorRegUpdate(1, is);

      break;
  }

}

float echoCM(int n) {
  digitalWrite(trigPin[n], LOW);  // Make sure the pin starts off low
  delayMicroseconds(2);                    // Min between triggers is 50us so let's do 1000us
  digitalWrite(trigPin[n], HIGH); // Set trigger pin High for....
  delayMicroseconds(5);       // 10 microseconds.
  digitalWrite(trigPin[n], LOW);  // Set it low again

  echoTime = pulseIn(echoPin[n], HIGH);  //Collect the echo pulse
  CMs = echoTime / 58;                //Conversion in Data Sheet
  Serial.println("Distance in Cms " + (String)n + "= " + (String)CMs);    // print data
  return CMs;
}
