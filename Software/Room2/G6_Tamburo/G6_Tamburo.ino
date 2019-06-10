//Teensy LC

#define ONLINE

#define SENNUM  1       //total amount of sensors
#define ACTNUM  0       //total amount of actuators
#define DEVNUM  3       //total amount of internal devices
#define ALWAYSACTIVE 0  //1 if the game is always active

const int senPins[SENNUM] = {23}; //
const int actPins[ACTNUM] = {};
const int devPins[DEVNUM] = {3, 4, 5};

//04:E9:E5:06:66:F4
uint8_t mac[] = {0x04, 0xE9, 0xE5, 0x06, 0x66, 0xF4}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
uint8_t ip[] = {10, 0, 1, 106};                     //This needs to be unique in your network - only one puzzle can have this IP

uint8_t stato = 0;

void resetSpec() {
  stato = 0;
}

#include <EscapeFunction.h>

const int knockSensor = 23;         // Piezo sensor on pin 0.

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 300;           // Minimum signal from the piezo to register as a knock
const int rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 15; // If the average timing of the knocks is off by this percent we don't unlock.
const int knockFadeTime = 100;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)

const int maximumKnocks = 50;       // Maximum number of knocks to listen for.
const int knockComplete = 1500;     // Longest time to wait for a knock before we assume that it's finished.

// Variables.
int secretCode[3][maximumKnocks] =
{ {100, 30, 90, 10, 20, 10, 90, 10, 20, 10, 10, 10, 10, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {70, 100, 100, 40, 60, 40, 60, 100, 100, 40, 60, 40, 60, 100, 100, 40, 60, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {35, 35, 35, 75, 100, 40, 70, 35, 35, 35, 35, 75, 100, 40, 70, 35, 35, 35, 35, 75, 100, 40, 70, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.
int knockSensorValue = 0;           // Last reading of the knock sensor.

void setup()
{
  setupEscape();
}

void loop()
{
  Mb.Run();
  listenFromEth();
  if (!triggered && gameActivated) {
    gameUpdate();
    isPuzzleSolved();
  }
  //printRegister();
}

void gameUpdate() {
  // Listen for any knock at all.
  knockSensorValue = analogRead(knockSensor);

  if (knockSensorValue >= threshold) {
    listenToSecretKnock();
  }
  if (stato > 2) puzzleSolved = true;
}

// Records the timing of knocks.
void listenToSecretKnock() {
  Serial.println("knock starting");
  int i = 0;
  // First lets reset the listening array.
  for (i = 0; i < maximumKnocks; i++) {
    knockReadings[i] = 0;
  }

  int currentKnockNumber = 0;             // Incrementer for the array.
  int startTime = millis();               // Reference for when this knock started.
  int now = millis();

  myDelay(knockFadeTime);                                 // wait for this peak to fade before we listen to the next one.

  do {
    Mb.Run();
    sensorRegUpdate(0, 1);
    //listen for the next knock or wait for it to timeout.
    knockSensorValue = analogRead(knockSensor);
    if (knockSensorValue >= threshold) {                 //got another knock...
      //record the delay time.
      Serial.println(knockSensorValue);
      now = millis();
      knockReadings[currentKnockNumber] = now - startTime;
      currentKnockNumber ++;                             //increment the counter
      startTime = now;
      myDelay(knockFadeTime);                              // again, a little delay to let the knock decay.
    }
    now = millis();
    //did we timeout or run out of knocks?
  } while ((now - startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  sensorRegUpdate(0,0);
  //we've got our knock recorded, lets see if it's valid
  if (validateKnock() == true) {
    deviceRegUpdate(stato, 1);   
    stato++;
    Serial.println("play sequence " + (String)stato); 
  } else {
    deviceRegUpdate(stato, 0);
    Serial.println("Secret knock failed.");
  }
}

// Sees if our knock matches the secret.
// returns true if it's a good knock, false if it's not.
// todo: break it into smaller functions for readability.
boolean validateKnock() {
  int i = 0;

  // simplest check first: Did we get the right number of knocks?
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;               // We use this later to normalize the times.

  for (i = 0; i < maximumKnocks; i++) {
    if (knockReadings[i] > 0) {
      currentKnockCount++;
      Serial.println("knock " + (String)i + ": " + (String)knockReadings[i]);
    }
    if (secretCode[stato][i] > 0) {          //todo: precalculate this.
      secretKnockCount++;
    }

    if (knockReadings[i] > maxKnockInterval) {  // collect normalization data while we're looping.
      maxKnockInterval = knockReadings[i];
    }
  }
  // Solo per riconoscere nuove sequenze
  Serial.println("-- lettura --");
  for (i = 0; i < maximumKnocks; i++) { // Normalize the times
    int knock = map(knockReadings[i], 0, maxKnockInterval, 0, 100);
    Serial.println("knock " + (String)i + ": " + (String)knock);
  }
  Serial.println("-- fine lettura --");

  if (currentKnockCount != secretKnockCount) {
    Serial.println("Wrong beat number");
    return false;

  }

  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
      (ie: if you do the same pattern slow or fast it should still open the door.)
      This makes it less picky, which while making it less secure can also make it
      less of a pain to use if you're tempo is a little slow or fast.
  */
  int totaltimeDifferences = 0;
  int timeDiff = 0;
  for (i = 0; i < maximumKnocks; i++) { // Normalize the times
    knockReadings[i] = map(knockReadings[i], 0, maxKnockInterval, 0, 100);
    Serial.println("knock " + (String)i + ": " + (String)knockReadings[i]);
    timeDiff = abs(knockReadings[i] - secretCode[stato][i]);
    if (timeDiff > rejectValue) { // Individual value too far out of whack
      Serial.println("Individual beat " + (String)i + " too far out of whack");
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences / secretKnockCount > averageRejectValue) {
    Serial.println("the whole thing is too inaccurate");
    return false;
  }
  return true;
}
