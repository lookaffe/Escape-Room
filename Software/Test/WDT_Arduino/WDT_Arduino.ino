#include <Adafruit_SleepyDog.h>

const int           led                 = 13;

bool                ledState            = false;
unsigned long       timer;

void setup()
{ // initialize the digital pin as an output.
  pinMode(led, OUTPUT);

  // Indicate we are starting over by hold led off for 1s
  ledState = false;
  digitalWrite(led, ledState);
  delay(1000UL);

  // Indicate we are in setup by hold LED on
  ledState = true;
  digitalWrite(led, ledState);
  delay(5000UL);
  //attivo il watchdog e lo imposto
  //per una soglia di tempo di 4 Secondi
  //
  //Watchdog.enable(1000);
  //delay(1);
}

void loop()
{
  timer = millis() + 10000UL;                                 // length of time we will reset WDT

  while (true) {
    ledState = !ledState;
    digitalWrite(led, ledState);
    delay(100UL);
    if (millis() < timer) {                                 // Have we timed out yet?
      //resetto il watchdog
      Watchdog.reset();
    }
  } // while

}
