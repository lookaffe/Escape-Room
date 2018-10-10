// ARDUINO MEGA (non usare pin 20, 21, 50, 51, 52, 53)

#include <Ethernet.h>
#include "CapacitiveSensor.h"

//------------------------ERM-----------------------------------
bool useDHCP = false;

byte mac[] = {0x04, 0xE9, 0xE5, 0x04, 0x04, 0x04}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
IPAddress ip(10, 0, 0, 104);                       //This needs to be unique in your network - only one puzzle can have this IP

/*
    Initialize the Ethernet server library
    with the IP address and port you want to use
   (port 80 is default for HTTP):
*/
EthernetServer server(80);

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?

//--------------------------------------------------------------

const int LEVEL = 24;
int minLevel = 400;
CapacitiveSensor   mano01 = CapacitiveSensor(24, 26);
CapacitiveSensor   mano02 = CapacitiveSensor(24, 27);
CapacitiveSensor   mano03 = CapacitiveSensor(24, 28);
CapacitiveSensor   mano04 = CapacitiveSensor(24, 29);
CapacitiveSensor   mano05 = CapacitiveSensor(24, 30);
CapacitiveSensor   mano06 = CapacitiveSensor(24, 31);
CapacitiveSensor   mano07 = CapacitiveSensor(24, 32);
CapacitiveSensor   mano08 = CapacitiveSensor(24, 33);
CapacitiveSensor   mano09 = CapacitiveSensor(24, 34);
CapacitiveSensor   mano10 = CapacitiveSensor(24, 35);
CapacitiveSensor   mano11 = CapacitiveSensor(24, 36);
CapacitiveSensor   mano12 = CapacitiveSensor(24, 37);
CapacitiveSensor   piede01 = CapacitiveSensor(24, 38);
CapacitiveSensor   piede02 = CapacitiveSensor(24, 39);
CapacitiveSensor   piede03 = CapacitiveSensor(24, 40);
CapacitiveSensor   piede04 = CapacitiveSensor(24, 41);
CapacitiveSensor   piede05 = CapacitiveSensor(24, 42);
CapacitiveSensor   piede06 = CapacitiveSensor(24, 43);
CapacitiveSensor   piede07 = CapacitiveSensor(24, 44);
CapacitiveSensor   piede08 = CapacitiveSensor(24, 45);
CapacitiveSensor   piede09 = CapacitiveSensor(24, 46);
CapacitiveSensor   piede10 = CapacitiveSensor(24, 47);
CapacitiveSensor   piede11 = CapacitiveSensor(24, 48);
CapacitiveSensor   piede12 = CapacitiveSensor(24, 49);

long push = 0;
int sequence[LEVEL] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int yourSequence[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long pushed[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int quadroPin = 17;
int botolaPin = 18;

void setup()
{
  //------------------------ERM-----------------------------------
  // Inizializzazione e test funzionamento Ethernet
  Serial.begin(9600);
  Serial.println("Serial interface started");  //Serial interface is for debugging/demo

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); // reset the WIZ820io
  delay(1000);
  digitalWrite(9, HIGH); // release the WIZ820io

  if (useDHCP) {
    Serial.println("Using DHCP");
    Ethernet.begin(mac); // Use DHCP instead of ip
  }
  else {
    Serial.println("Not using DHCP");
    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Initial game state
  puzzleSolved = false;

  //--------------------------------------------------------------


  mano01.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano02.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano03.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano04.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano05.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano06.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano07.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano08.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano09.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano10.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano11.set_CS_AutocaL_Millis(0xFFFFFFFF);
  mano12.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede01.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede02.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede03.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede04.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede05.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede06.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede07.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede08.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede09.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede10.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede11.set_CS_AutocaL_Millis(0xFFFFFFFF);
  piede12.set_CS_AutocaL_Millis(0xFFFFFFFF);

  pinMode(quadroPin, OUTPUT);
  pinMode(botolaPin, OUTPUT);
  digitalWrite(quadroPin, HIGH);
  digitalWrite(botolaPin, HIGH);

}

void loop()
{
  pushed[0] =  mano01.capacitiveSensor(30);
  pushed[1] =  mano02.capacitiveSensor(30);
  pushed[2] =  mano03.capacitiveSensor(30);
  pushed[3] =  mano04.capacitiveSensor(30);
  pushed[4] =  mano05.capacitiveSensor(30);
  pushed[5] =  mano06.capacitiveSensor(30);
  pushed[6] =  mano07.capacitiveSensor(30);
  pushed[7] =  mano08.capacitiveSensor(30);
  pushed[8] =  mano09.capacitiveSensor(30);
  pushed[9] =  mano10.capacitiveSensor(30);
  pushed[10] =  mano11.capacitiveSensor(30);
  pushed[11] =  mano12.capacitiveSensor(30);
  pushed[12] =  piede01.capacitiveSensor(30);
  pushed[13] =  piede02.capacitiveSensor(30);
  pushed[14] =  piede03.capacitiveSensor(30);
  pushed[15] =  piede04.capacitiveSensor(30);
  pushed[16] =  piede05.capacitiveSensor(30);
  pushed[17] =  piede06.capacitiveSensor(30);
  pushed[18] =  piede07.capacitiveSensor(30);
  pushed[19] =  piede08.capacitiveSensor(30);
  pushed[20] =  piede09.capacitiveSensor(30);
  pushed[21] =  piede10.capacitiveSensor(30);
  pushed[22] =  piede11.capacitiveSensor(30);
  pushed[23] =  piede12.capacitiveSensor(30);


  Serial.println();
  for (int x = 0; x < LEVEL; x++) {
    if (pushed[x] > minLevel) { //pushed[x] > 400)
      yourSequence[x] = 1;
    } else yourSequence[x] = 0;
    Serial.print(yourSequence[x]); Serial.print(" ");
  }

  isPuzzleSolved();
}

//compare the two sequences
boolean seq_cmp(int *a, int *b) {
  for (int n = 0; n < LEVEL; n++) if (a[n] != b[n]) return false;
  return true;
}

//------------------------ERM-----------------------------------
/*
    Controlla se il puzzle è risolto e invia dato a ERM
*/
void isPuzzleSolved() {
  if (seq_cmp(yourSequence, sequence)) {
    Serial.print("OK");
    puzzleSolved = true;
    digitalWrite(quadroPin, LOW);
    digitalWrite(botolaPin, LOW);
  } else {
    puzzleSolved = false;
    digitalWrite(quadroPin, HIGH);
    digitalWrite(botolaPin, HIGH);
  }
}

// Azione su ricezione comando "trigger" da ERM
void trigger() {
  digitalWrite(quadroPin, LOW);
  digitalWrite(botolaPin, LOW);;

}

// Resetta il gioco
void reset() {
  digitalWrite(quadroPin, HIGH);
  digitalWrite(botolaPin, HIGH);
  puzzleSolved = false;
}
/*
   URL routing for prop commands and polling
*/

// This function dictates what text is returned when the prop is polled over the network
String statusString() {
  return puzzleSolved ? "solved" : "not solved";
}

// Actual request handler
void processRequest(EthernetClient& client, String requestStr) {
  Serial.println(requestStr);
  // Send back different response based on request string
  if (requestStr.startsWith("GET /status")) {
    Serial.println("polled for status!");
    writeClientResponse(client, statusString());
  } else if (requestStr.startsWith("GET /reset")) {
    Serial.println("Room reset");
    reset();
    writeClientResponse(client, "ok");
  } else if (requestStr.startsWith("GET /trigger")) {
    Serial.println("Network prop trigger");
    trigger();
    writeClientResponse(client, "ok");
  } else {
    writeClientResponseNotFound(client);
  }
}


/*
   HTTP helper functions
*/

void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // Grab the first HTTP header (GET /status HTTP/1.1)
    String requestStr;
    boolean firstLine = true;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          processRequest(client, requestStr);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          firstLine = false;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;

          if (firstLine) {
            requestStr.concat(c);
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}


void writeClientResponse(EthernetClient& client, String bodyStr) {
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}


void writeClientResponseNotFound(EthernetClient& client) {
  // send a standard http response header
  client.println("HTTP/1.1 404 Not Found");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
}

//--------------------------------------------------------------


