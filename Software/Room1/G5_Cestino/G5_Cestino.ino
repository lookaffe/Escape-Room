// Teensy LC
// servono jumoer maschio femmina di almeno 30 cm oppure una schedina collettore di GND, VCC e 6 segnali per rimandare al teensy

#define SENSORCNT 5

const int SENS1 = 23; //Puzzle number
const int SENS2 = 22; //Puzzle number
const int SENS3 = 21; //Puzzle number
const int SENS4 = 20; //Puzzle number
const int SENS5 = 19; //Puzzle number
const int SENS6 = 18; //Puzzle number

int val = 100;

void setup()
{
  Serial.begin(9600);
  pinMode(SENS1, INPUT);
  pinMode(SENS2, INPUT);
  pinMode(SENS3, INPUT);
  pinMode(SENS4, INPUT);
  pinMode(SENS5, INPUT);
  pinMode(SENS6, INPUT);
  
}

void loop()
{ 
  int val1 = analogRead(SENS1);
  int val2 = analogRead(SENS2);
  int val3 = analogRead(SENS3);
  int val4 = analogRead(SENS4);
  int val5 = analogRead(SENS5);
  int val6 = analogRead(SENS6);
  Serial.print("analog 1 is: "); Serial.print(val1);
  Serial.print(" - 2 is: "); Serial.print(val2);
  Serial.print(" - 3 is: "); Serial.print(val3);
  Serial.print(" - 4 is: "); Serial.print(val4);
  Serial.print(" - 5 is: "); Serial.print(val5);
  Serial.print(" - 6 is: "); Serial.print(val6);
  Serial.println();

  if(val1<val || val2<val|| val3<val || val4<val || val5<val || val6<val) puzzleSolved(true);
  delay(100);
}

void puzzleSolved(boolean sol){
  Serial.println("Puzzle Solved!");
  }

