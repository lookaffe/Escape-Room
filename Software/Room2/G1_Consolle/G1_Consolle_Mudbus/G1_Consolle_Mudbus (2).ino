//Teensy 3.2

#define ONLINE

#define SENNUM  4       //total amount of sensors
#define ACTNUM  3       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

//Stati
#define TASTO 0     // stato in attesa della pressione del TASTO
#define ACQUA 1     // stato in attesa della pressione dell'ACQUA
#define FIACCOLA 2  // stato in attesa della pressione della FIACCOLA
#define CIAMBELLA 3 // stato in attesa della pressione della CIAMBELLA
#define END 4

#define SMOKEINTERVAL 5000  // intervallo tra una fumata e l'altra
#define WATERLEVEL 50      // valore per presenza acqua

const int senPins[SENNUM] = {21, 23, 17, 16}; // pulsanti, acqua, fiaccola, ingranaggio
const int actPins[ACTNUM] = {2, 4, 6}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

//04:E9:E5:07:A5:85
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x07, 0xA5, 0x85}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};                     //This needs to be unique in your network - only one puzzle can have this IP

#include <Bounce.h>
Bounce buttonsStart = Bounce(senPins[0], 100);
Bounce fiaccola = Bounce(senPins[2], 100);
Bounce ingranaggio = Bounce(senPins[3], 100);

#include <Servo.h>
//

long smokeStartTime, smokeTime;
bool orologio = true;
int water = 0;

uint8_t stato = TASTO;

void resetSpec() {
  stato = 0;
}

#include <EscapeFunction.h>

void setup()
{
  setupEscape();
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }

  //printRegister();
}

void gameUpdate() {
  bool pressed = 0;
  switch (stato) {
    case TASTO:
      Serial.print("STATo "); Serial.println(stato);
      buttonsStart.update();
      pressed = buttonsStart.fallingEdge();
      sensorRegUpdate(stato, pressed);
      if (pressed) stato = ACQUA;
      break;

    case ACQUA:
      Serial.print("STATo "); Serial.println(stato);
      smokeTime = millis();
      if (orologio) {
        //tutti i giri da fare con il motore per l'orologio
        myDelay(3000);
        orologio = false;
        smokeStartTime, smokeTime = millis();
      }
      if (smokeTime - smokeStartTime > SMOKEINTERVAL) {
        smoke();
        smokeStartTime = millis();
      }
      water = analogRead(senPins[1]);
      sensorRegUpdate(stato, water);
      Serial.print("Acqua "); Serial.println(water);
      if (water > WATERLEVEL) stato = FIACCOLA;
      break;

    case FIACCOLA:
      Serial.print("STATo "); Serial.println(stato);
      fiaccola.update();
      pressed = fiaccola.fallingEdge();
      Serial.print("pressed "); Serial.println(pressed);
      sensorRegUpdate(stato, pressed);
      if (pressed) {
        actuatorRegUpdate(1, HIGH);
        stato = CIAMBELLA;
      }
      break;

    case CIAMBELLA:
      Serial.print("STATo "); Serial.println(stato);
      ingranaggio.update();
      pressed = ingranaggio.fallingEdge();
      Serial.print("pressed "); Serial.println(pressed);
      sensorRegUpdate(stato, pressed);
      if (pressed) {
        actuatorRegUpdate(2, HIGH); // attiva il motore degli ingranaggi
        // fai muovere l'orologio
        puzzleSolved = 1;
        stato = 4;
      }
      break;

    case END:
      break;
  }
}

void smoke() {
  Serial.println("FUMO!");
  actuatorRegUpdate(0, HIGH);
  myDelay(1000);
  actuatorRegUpdate(0, LOW);
}



