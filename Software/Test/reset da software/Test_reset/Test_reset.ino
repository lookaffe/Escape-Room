#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define DEVICE 1
int led = 2;

void setup() {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  for (int i = 0; i < 10; i++) {
    digitalWrite(led, !digitalRead(led));
    delay(50);
  }
  Serial.println("Setup");
  delay(500);
}

void loop() {
  Serial.println("loop");
  delay(5000);Serial.println("loop");
  delay(5000);Serial.println("loop");
  delay(5000);

  if(DEVICE) {
    asm volatile (" nop ");
    asm volatile (" jmp 0 ");
  }
    else CPU_RESTART;
  Serial.println("looopa");
}
