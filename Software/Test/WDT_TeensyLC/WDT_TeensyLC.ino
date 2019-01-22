const int           led                 = 13;
bool                ledState            = false;
unsigned long       timer;
extern "C" void startup_early_hook() {}
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  // blink a few times
  for (int i = 0; i < 5; i++) {
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);

    // service the COP
    SIM_SRVCOP = 0x55;
    SIM_SRVCOP = 0xAA;
  }

  // COP is not serviced, the Teensy LC will reset within 1 sec
}

void loop() {
  timer = millis() + 10000UL;
  while (true) {
    ledState = !ledState;
    digitalWrite(led, ledState);
    delay(100UL);
    if (millis() < timer) {                                 // Have we timed out yet?
    }
  } // while
}

// redefine the startup_early_hook which by default
//  disables the COP


