//Teensy 3.2

#define ONLINE

#define SENNUM  2       //total amount of sensors
#define ACTNUM  3       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 0  //1 if the game is always active

const int senPins[SENNUM] = {0, 23}; // Ultrasuono alto, Ultrasuono basso
const int actPins[ACTNUM] = {17, 5, 6}; // relay luce alta, relay luce bassa elettrovalvola nuvola
const int devPins[DEVNUM] = {}; // luce alta, luce bassa
// 04:E9:E5:06:E1:29
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 107};                     //This needs to be unique in your network - only one puzzle can have this IP

int stato = 0;
bool passUp = false;
bool passDo = false;
const long rainTime = 2000; // tempo di apertura elettrovalvola pioggia
const float distance = 70.00; // distanza minima di rilevamento ultrasuono
elapsedMillis lapUp, lapDown = 0;
const long lapTime = 3000; // tempo massimo per un giro
elapsedMillis danceUp, danceDown = 0;
const long danceTime = 10000; // tempo minimo di danza
int count = 0;

void resetSpec() {
  stato = 0; passUp = false; passDo = false;
}

byte trigPin[2] = {1, 22};  // Trigger is connected to this pin
byte echoPin[2] = {0, 23};   // Echo is connected to this pin
float echoTime;       // Variable to store echo pulse time
float CMs;            // Variable to carry Cm values

#include <EscapeFunction.h>

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
  if (!puzzleSolved && gameActivated) {
    gameUpdate();
    isPuzzleSolved();
  }
  printRegister();
}

void gameUpdate() {
  float dist[2] = {0.0, 0.0};
  for (int i = 0; i < 2; i++) {
    dist[i] = echoCM(i);    // Call non-void function to get the distance in cm
    myDelay(100);             // Delay 1/2 second
  }
  //Serial.println("lapUp " + (String)lapUp);
  switch (stato) {
    case 0:
      stato = Mb.R[ACTIVE];
      lapUp = 0;
      danceUp = 0;
      actuatorRegUpdate(0, stato); // accendo la luce alta
      break;
    case 1:
      passUp = false;
      if (dist[0] < distance) { // 1. se passo davanti al sensore alto
        passUp = true;
        if (lapUp < lapTime) { // 3. se il tempo del giro è minore di quello stabilito (non sei lento) e
          if (danceUp > danceTime) {
            stato = 2; // 4. Se il tempo di danza è maggiore di quello stabilito (hai ballato abbastanza) passo allo stato 2
            lapDown = 0;
            danceDown = 0;
            actuatorRegUpdate(0, 0);// spengo luce alta
            actuatorRegUpdate(1, 1); // accendo luce bassa
          }
        } else {
          danceUp = 0; // 5. altrimenti aggiorno il tempo di ballo
        }
        lapUp = 0; // 2. comunque aggiorno il tempo del giro
      }
      sensorRegUpdate(0, passUp);
      break;
    case 2:
      passDo = false;
      sensorRegUpdate(0, 0); // resetto il registro dell'ultrasuono alto
      if (dist[0] < distance && lapUp > 1500) {
        stato = 1; // 0. se passo davanti al sensore alto, ritorno allo stato 1
        sensorRegUpdate(1, 0); // resetto il registro dell'ultrasuono basso
        actuatorRegUpdate(0, 1); // accendo luce alta
        actuatorRegUpdate(1, 0);  // spengo luce bassa
      }
      if (dist[1] < distance) {
        passDo = true;// 1. se passUpo davanti al sensore basso
        if (lapDown < lapTime) { // 3. se il tempo del giro è minore di quello stabilito (non sei lento)
          if (danceDown > danceTime) {
            stato = 3;  // 4. Se il tempo di danza è maggiore di quello stabilito (hai ballato abbastanza)
          }
        } else {
          danceDown = 0; // 5. altrimenti aggiorno il tempo di ballo
        }
        lapDown = 0; // 2. comunque aggiorno il tempo del giro
      }
      sensorRegUpdate(1, passDo);
      break;
      case 3:
      actuatorRegUpdate(1, 0);  // spengo luce bassa
      actuatorRegUpdate(2, 1); // apro elettrovalvola
      myDelay(rainTime);
      actuatorRegUpdate(2, 0); // chiudo elettrovalvola
      puzzleSolved = 1;
  }
}

float echoCM(int n) {
  digitalWrite(trigPin[n], LOW);  // Make sure the pin starts off low
  delayMicroseconds(2);                    // Min between triggers is 50us so let's do 1000us
  digitalWrite(trigPin[n], HIGH); // Set trigger pin High for....
  delayMicroseconds(10);       // 10 microseconds.
  digitalWrite(trigPin[n], LOW);  // Set it low again

  echoTime = pulseIn(echoPin[n], HIGH);  //Collect the echo pulse
  CMs = echoTime / 58;                //Conversion in Data Sheet
  Serial.println("Distance in Cms " + (String)n + "= " + (String)CMs);    // print data
  return CMs;
}
