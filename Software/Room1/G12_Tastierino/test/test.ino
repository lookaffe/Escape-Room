
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad
int date = 11281;
long prevPressTime = 0;

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  prevPressTime = millis();
}

void loop() {
  int vv = getNumber();
  (date == vv) ? Serial.println("Yes") : Serial.println("No");
}

int getNumber() {
  prevPressTime = millis();
  int num = 0;
  char key = kpd.getKey();
  while (millis() - prevPressTime < 3000) {
    switch (key) {
      case NO_KEY:
        break;

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        prevPressTime = millis();
        Serial.print(key);
        num = num * 10 + (key - '0');
        break;

      case '*':case '#':
        num = 0;
        Serial.println();
        break;
    }
    key = kpd.getKey();
  }
  return num;
}
