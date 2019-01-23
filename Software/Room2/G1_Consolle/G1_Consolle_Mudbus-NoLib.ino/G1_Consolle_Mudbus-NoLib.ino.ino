#include <SPI.h>
#include <Ethernet.h>
#include <Mudbus.h>
#include <Adafruit_SleepyDog.h>

// Restart define for Teensy
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#if defined(ARDUINO_ARCH_AVR) || defined(__AVR__) || defined(ARDUINO_ARCH_SAMD) // Arduino board

#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) // Teensy 3.* board

#elif defined(__MKL26Z64__) // Teensy LC board

#endif

#define SENNUM  4       //total amount of sensors
#define ACTNUM  4       //total amount of actuators
#define DEVNUM  0       //total amount of internal devices
#define ALWAYSACTIVE 1  //1 if the game is always active

//Stati
#define TASTO 0     // stato in attesa della pressione del TASTO
#define ACQUA 1     // stato in attesa della pressione dell'ACQUA
#define FIACCOLA 2  // stato in attesa della pressione della FIACCOLA
#define CIAMBELLA 3 // stato in attesa della pressione della CIAMBELLA

#define SMOKEINTERVAL 5000  // intervallo tra una fumata e l'altra
#define WATERLEVEL 50      // valore per presenza acqua

const int senPins[SENNUM] = {21, 20, 19, 18}; // acqua, pulsanti, fiaccola, ingranaggio
const int actPins[ACTNUM] = {0, 1, 2}; // relayFumo, relaySportello, relayIngranaggi
const int devPins[DEVNUM] = {};

uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xE1, 0x29}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 101};

//Modbus Registers Offsets (0-9999)
const int STATE = 0;
const int RESTART = 99;
const int RESET = 100;
const int ACTIVE = 124;

int SENSORS[SENNUM];
int ACTUATORS[ACTNUM];
int DEVICES[DEVNUM];

int sensStatus[SENNUM];

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?
bool triggered = false; // has the control room triggered some actuator?
bool gameActivated = ALWAYSACTIVE; // is the game active?
#include <Bounce.h>
Bounce buttonsStart = Bounce(senPins[1], 100);
Bounce fiaccola = Bounce(senPins[2], 100);
Bounce ingranaggio = Bounce(senPins[3], 100);

#include <Servo.h>
//ModbusIP object
Mudbus Mb;
long smokeStartTime, smokeTime;
bool orologio = true;
int water = 0;

uint8_t stato = TASTO;
void myDelay(unsigned long d) {
  unsigned long t = millis();
  while (millis() < t + d) {
    Mb.Run();
    Watchdog.reset();
    delay(1);
  }
}
void setup()
{
  setupEscape();
}
void loop()
{
  listenFromEth();
  if (!triggered) {
    gameUpdate();
    isPuzzleSolved();
  }

  //printRegister();
}

void gameUpdate() {
  switch (stato) {
    case TASTO:
      Serial.print("STATo "); Serial.println(stato);
      buttonsStart.update();
      if (buttonsStart.fallingEdge()) stato = ACQUA;
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
      water = analogRead(senPins[0]);
      sensorRegUpdate(0, water);
      Serial.print("Acqua "); Serial.println(water);
      if (water > WATERLEVEL) stato = FIACCOLA;
      break;
    case FIACCOLA:
      Serial.print("STATo "); Serial.println(stato);
      fiaccola.update();
      if (fiaccola.fallingEdge()) {
        digitalWrite(actPins[1],LOW); // apri sportello ingranaggi
        stato = CIAMBELLA;
      }
      break;
    case CIAMBELLA:
      Serial.print("STATo "); Serial.println(stato);
           ingranaggio.update();
      if (ingranaggio.fallingEdge()) {
        digitalWrite(actPins[2],LOW); // attiva il motore degli ingranaggi
        // fai muovere l'orologio
        stateRegUpdate(HIGH);
        triggered = HIGH;
      }
      break;
  }
  //  sensorRegUpdate(0, puzzleSolved);
  //  sensorRegUpdate(1, digitalRead(senPins[1]));
}

void smoke() {
  Serial.println("FUMO!");
  digitalWrite(actPins[0], LOW);
  actuatorRegUpdate(0, HIGH);
  myDelay(1000);
  digitalWrite(actPins[0], HIGH);
  actuatorRegUpdate(0, LOW);
}

void setupEscape(){
  Serial.begin(9600);
  // reset for Ethernet Shield
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  for(int i = 0; i < 100; i++){
  delay(10);
  Watchdog.reset();
  }
  digitalWrite(9, HIGH); // release the WIZ820io

  Ethernet.begin(mac, ip);
  for(int i = 0; i < 100; i++){
  delay(50);
  Watchdog.reset();
  }

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = false;
   Mb.R[STATE] = puzzleSolved;
  Mb.R[ACTIVE] = gameActivated;

  for (uint8_t i = 0; i < SENNUM; i++) {
  SENSORS[i] = i + 1;
  sensStatus[i] = LOW;
  }

  for (uint8_t i = 0; i < ACTNUM; i++) {
  ACTUATORS[i] = i + 101;
  }

  for (uint8_t i = 0; i < DEVNUM; i++) {
  DEVICES[i] = i + 51;
  }

  // Configure the pins for input mode with pullup resistors.
  for (int i = 0; i < SENNUM ; i++) {
  pinMode(senPins[i], INPUT);
  }

  for (int i = 0; i < ACTNUM ; i++) {
  pinMode(actPins[i], OUTPUT);
  digitalWrite(actPins[i], HIGH); // Open on LOW
  }

  for (int i = 0; i < DEVNUM ; i++) {
  pinMode(devPins[i], OUTPUT);
  digitalWrite(devPins[i], LOW);
  }
  Watchdog.enable(8000);
}

void sensorRegUpdate(int reg, int val)
{
  Mb.R[SENSORS[reg]] = val;
}

void actuatorRegUpdate(int reg, int val)
{
    Mb.R[ACTUATORS[reg]] = val;
}

void deviceRegUpdate(int reg, int val)
{
    Mb.R[DEVICES[reg]] = val;
}

void stateRegUpdate(int val)
{
    Mb.R[STATE] = val;
}

int sensorRegRead(int reg)
{
    return Mb.R[SENSORS[reg]];
}

int actuatorRegRead(int reg)
{
    return Mb.R[ACTUATORS[reg]];
}

int deviceRegRead(int reg)
{
    return Mb.R[DEVICES[reg]];
}

int stateRegRead()
{
    return Mb.R[STATE];
}

// Azione su ricezione comando "trigger"
void trigger(int actPin, boolean trig) {
  Mb.R[ACTUATORS[actPin]] = trig;
  triggered = trig;
  digitalWrite(actPins[actPin], !trig);
  delay(10);
}

void isPuzzleSolved() {
  trigger(0, puzzleSolved);
  Mb.R[STATE] = puzzleSolved;
  triggered = puzzleSolved;
}

void reset() {
  for (int i = 0; i < ACTNUM ; i++) {
  trigger(i, LOW);
  }
  for (int i = 0; i < SENNUM ; i++) {
  sensStatus[i] = LOW;
  Mb.R[SENSORS[i]] = sensStatus[i];
  }
  for (int i = 0; i < DEVNUM ; i++) {
  digitalWrite(devPins[i], LOW);
  Mb.R[DEVICES[i]] = LOW;
  }
  triggered = false;
  puzzleSolved = false;
  Mb.R[STATE] = puzzleSolved;
  Mb.R[RESET] = LOW;
  if (!ALWAYSACTIVE) {
  gameActivated = false;
  Mb.R[ACTIVE] = gameActivated;
  }

  #ifdef SPECRESET
  resetSpec();
  #endif
}

void listenFromEth() {
  Watchdog.reset();
  if (Mb.R[RESET]) reset();
  else {
  triggered = Mb.R[STATE];
  for (int i = 0; i < SENNUM ; i++) {
    sensStatus[i] = Mb.R[SENSORS[i]];
  }
  for (int i = 0; i < ACTNUM ; i++) {
    trigger(i, Mb.R[ACTUATORS[i]]);
    triggered = triggered || Mb.R[ACTUATORS[i]];
  }
  for (int i = 0; i < DEVNUM ; i++) {
    digitalWrite(devPins[i], Mb.R[DEVICES[i]]);
  }
  puzzleSolved = Mb.R[STATE];
  if (Mb.R[STATE]) {
    for (int i = 0; i < ACTNUM ; i++) {
    trigger(i, Mb.R[STATE]);
    }
  }
  gameActivated = Mb.R[ACTIVE];
  if(Mb.R[RESTART]) {
    #if defined(ARDUINO_ARCH_AVR) || defined(__AVR__) || defined(ARDUINO_ARCH_SAMD)
      asm volatile (" nop ");
      asm volatile (" jmp 0x0000");
    #elif defined (__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || (__MKL26Z64__)
      CPU_RESTART;
    #endif// WDT definition
  }
  }
}

void printRegister() {
  Serial.print("STATE (reg "); Serial.print(STATE); Serial.print(") - val:  "); Serial.println(Mb.R[STATE]);
  for (int i = 0; i < SENNUM; i++) {
  Serial.print("SENSORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(SENSORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[SENSORS[i]]);
  }
  for (int i = 0; i < ACTNUM; i++) {
  Serial.print("ACTUATORS "); Serial.print(i); Serial.print(" (reg "); Serial.print(ACTUATORS[i]); Serial.print(") - val:  "); Serial.println(Mb.R[ACTUATORS[i]]);
  }
  for (int i = 0; i < DEVNUM; i++) {
  Serial.print("DEVICES "); Serial.print(i); Serial.print(" (reg "); Serial.print(DEVICES[i]); Serial.print(") - val:  "); Serial.println(Mb.R[DEVICES[i]]);
  }
  Serial.print("ACTIVATION: "); Serial.println(Mb.R[ACTIVE]);
  Serial.println();
}


