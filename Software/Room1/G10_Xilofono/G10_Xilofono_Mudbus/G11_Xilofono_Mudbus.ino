// TEENSY LC
/*
 * Pin Tasto PLAY 19
 * Pin Tasto RESET 35
 * Pin Luce PLAY 21
 * Pin Luce RESET 33
 * Pin Relay Elettrovalvole 17, 15, 22, 20, 18, 16
 * Pin Martelletti 25, 24, 26, 28, 30, 32
 * Pin Servo 23
 */
const int MAX_LEVEL = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 4;

int velocity = 500;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);

  pinMode(5, OUTPUT);

  digitalWrite(5, LOW);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
}

void loop()
{
//  Serial.print("A0: "); Serial.println(digitalRead(A0));
//  Serial.print("A1: "); Serial.println(digitalRead(A1));
//  Serial.print("A2: "); Serial.println(digitalRead(A2));
//  Serial.print("A3: "); Serial.println(digitalRead(A3));
//  delay(velocity);
  if (level == 4)
    generate_sequence();//generate a sequence;

  if (deButton(A4, LOW)) //If start button is pressed
  {
    show_sequence();    //show the sequence
    get_sequence();     //wait for your sequence
  }
}

void show_sequence()
{
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);

  for (int i = 0; i < level; i++)
  {
    digitalWrite(sequence[i], HIGH);
    delay(velocity);
    digitalWrite(sequence[i], LOW);
    delay(200);
  }
}

void get_sequence()
{
  int flag = 0; //this flag indicates if the sequence is correct

  for (int i = 0; i < level; i++)
  {
    flag = 0;
    while (flag == 0)
    {
      if (deButton(A3, HIGH))
      {
        digitalWrite(6, HIGH);
        your_sequence[i] = 5;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(6, LOW);
      }

      if (deButton(A2, HIGH))
      {
        digitalWrite(4, HIGH);
        your_sequence[i] = 4;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(4, LOW);
      }

      if (deButton(A1, HIGH))
      {
        digitalWrite(3, HIGH);
        your_sequence[i] = 3;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(3, LOW);
      }

      if (deButton(A0, HIGH))
      {
        digitalWrite(2, HIGH);
        your_sequence[i] = 2;
        flag = 1;
        delay(velocity);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(2, LOW);
      }

    }
  }
  right_sequence();
}

void generate_sequence()
{
  randomSeed(millis()); //in this way is really random!!!

  for (int i = 0; i < MAX_LEVEL; i++)
  {
    sequence[i] = random(2, 5);
  }
}
void wrong_sequence()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(6, HIGH);
    delay(250);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(6, LOW);
    delay(250);
  }
  level = 4;
}

void right_sequence()
{
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
  delay(250);

  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
  delay(500);

//  if (level < MAX_LEVEL);
//  level++;
 digitalWrite(5, HIGH);

}

boolean deButton (int pin, boolean state)
{
  boolean stateNow = digitalRead(pin);
  if(state==stateNow) 
  {
    delay(10);
    return HIGH;
  } 
  else return LOW;
}

