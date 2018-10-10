//
/*This sketch is a simple version of the famous Simon Says game. You can use it and improved it adding
  levels and everything you want to increase the diffuculty!

  There are five buttons connected to A0, A1, A2, A3 and A4.
  The buttons from A0 to A3 are used to insert the right sequence while A4 to start the game.

  When a wrong sequence is inserted all the leds will blink for three time very fast otherwhise the
  inserted sequence is correct.

  Hardware needed:
  5x pushbuttons
  1x Blue led
  1x Yellow led
  1x Red led
  1x Green Led
  4x 1k resistors
  4x 10k resisors
  10x jumpers
*/

const int MAX_LEVEL = 100;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int velocity = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);

  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
}

void loop()
{
  if (level == 1)
    generate_sequence();//generate a sequence;

  show_sequence();    //show the sequence
  get_sequence();     //wait for your sequence
}

void show_sequence()
{
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);

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
      Serial.println(digitalRead(8));
      if (digitalRead(5) == LOW)
      {
        digitalWrite(4, HIGH);
        your_sequence[i] = 4;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(4, LOW);
      }

      if (digitalRead(6) == LOW)
      {
        digitalWrite(3, HIGH);
        your_sequence[i] = 3;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(3, LOW);
      }

      if (digitalRead(7) == LOW)
      {
        digitalWrite(2, HIGH);
        your_sequence[i] = 2;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(2, LOW);
      }

      if (digitalRead(8) == LOW)
      {
        digitalWrite(1, HIGH);
        your_sequence[i] = 1;
        flag = 1;
        delay(200);
        if (your_sequence[i] != sequence[i])
        {
          wrong_sequence();
          return;
        }
        digitalWrite(1, LOW);
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
    sequence[i] = random(2, 6);
  }
}
void wrong_sequence()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    delay(250);
    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    delay(250);
  }
  level = 1;
  velocity = 1000;
}

void right_sequence()
{
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  delay(250);

  digitalWrite(1, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  delay(500);

  if (level < MAX_LEVEL);
  level++;

  velocity -= 50; //increase difficulty
}
////Teensy 3.5
//
////#include <Ethernet.h>
//#include <Bounce.h>
//
//const int LEVEL = 12;
//
//int leds[] = {1, 2, 3, 4};
//int playPin = 23 ;
//int resetPin = 22;
//int playLed = 0;
//int resetLed = 1;
//int hammerPin[] = {5,6,7,8};//{24, 25, 26, 27, 28, 29};
//int waterBtnPin[] = {16, 17, 18, 19, 20, 21};
//int valvePin[] = {2, 3, 4, 5, 6, 7};
//int N = 0;
//int Nuser = 0;
//boolean userTurn = false;
//int combi[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 12
//int input[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 12
//
//int lastState[] = {LOW, LOW, LOW, LOW};
//boolean changed[] = {false, false, false, false};
//boolean push[] = {false, false, false, false};
//long time[] = {0, 0, 0, 0};
//long debounce = 30;
//
////Bounce button0 = Bounce(switchPin[0], 100);
////Bounce button1 = Bounce(switchPin[1], 100);  // 10 = 10 ms debounce time
////Bounce button2 = Bounce(switchPin[2], 100);  // which is appropriate for
////Bounce button3 = Bounce(switchPin[3], 100);  // most mechanical pushhammerPin
////Bounce button4 = Bounce(switchPin[4], 100);
////Bounce button5 = Bounce(switchPin[5], 100);
//
//void reset_push() {
//  for (int i = 0; i < 4; i++) {
//    push[i] = false;
//  }
//}
//
//void get_push() {
//  for (int i = 0; i < 4; i++) {
//    int current = digitalRead(hammerPin[i]);
//
//    if (current != lastState[i]) {
//      time[i] = millis(); // démarage du compteur
//
//      changed[i] = true;
//    }
//
//    if ((millis() - time[i]) > debounce && changed[i]) {
//      if (lastState[i] == HIGH) {
//        push[i] = true;
//        Serial.print("Appui ");
//        Serial.println(i+1);
//      }
//
//      changed[i] = false;
//    }
//
//    lastState[i] = current;
//  }
//}
//
//void blink() {
//  for (int i = 0; i < 4; i += 1) {
//    digitalWrite(leds[i], HIGH);
//  }
//  delay(1000);
//  for (int i = 0; i < 4; i += 1) {
//    digitalWrite(leds[i], LOW);
//  }
//  delay(1000);
//}
//
//void setup() {
//  Serial.begin(9600);
//  randomSeed(analogRead(0));
//
//  for (int i = 0; i < 4; i += 1) {
//    pinMode(leds[i], OUTPUT);
//    pinMode(hammerPin[i], INPUT);
//  }
//}
//
//void loop() {
//  boolean exit = false;
//
//  // All'inizio si genera una combinazione
//  if (N == 0) {
//    Serial.println("Generazione della combinazione");
//    blink();
//
//    Serial.print("Combinazione : ");
//    for (int i = 0; i < 12; i += 1) {
//      Serial.print(combi[i]+1);
//    }
//    Serial.println(".");
//    N = 1;
//    Nuser = 0;
//  }
//
//  // Turno del computer
//  if (!userTurn) {
//    Serial.println("Visualizzazione della combinazione");
//
//    for (int i = 0; i < N; i++) {
//      digitalWrite(leds[combi[i]], HIGH);
//      delay(300);
//      digitalWrite(leds[combi[i]], LOW);
//      delay(300);
//    }
//
//    userTurn = true;
//
//    reset_push();
//  }
//
//  // Turno utente
//  if (userTurn) {
//    // Per ogni voce
//    for (int i = 0; i < 4 && !exit; i++) {
//      if (push[i]) {
//        input[Nuser] = i;
//
//        if (input[Nuser] != combi[Nuser]) {
//          Serial.println("Sbagliato");
//          exit = true;
//          N = 0;
//          userTurn = false;
//        }
//
//        Nuser += 1;
//        push[i] = false;
//      }
//    }
//
//    if (Nuser >= N && !exit) {
//      Serial.println("Fine");
//      delay(1000); // Attend avant d'afficher une nouvelle combinaison
//      Nuser = 0;
//      N += 1;
//      if (N >= 12) {
//        Serial.println("Vinto");
//        blink();
//        blink();
//        blink();
//        N = 0;
//      }
//
//      userTurn = false;
//    }
//  }
//
//  get_push();
//}
