//Teensy LC

#include <Ethernet.h>
#include <Bounce.h>

//------------------------ERM-----------------------------------
bool useDHCP = false;

byte mac[] = {0x04, 0xE9, 0xE5, 0x06, 0xDA, 0x9A}; //Dipende da ogni dispositivo, da trovare con T3_readmac.ino (Teensy) o generare (Arduino)
IPAddress ip(10, 0, 0, 103);                       //This needs to be unique in your network - only one puzzle can have this IP

/* Serial.println(puzzleSolved)
    Initialize the Ethernet server library
    with the IP address and port you want to use
   (port 80 is default for HTTP):
*/
EthernetServer server(80);

// Track the room game state
bool puzzleSolved = false;  // has the puzzle in the room been solved?

//--------------------------------------------------------------

const int pinSwitch = 16;
Bounce button0 = Bounce(pinSwitch, 10); //Pin button
int StatoSwitch = 0;

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

   pinMode(pinSwitch, INPUT_PULLUP);
  
}

void loop()
{
  button0.update();

  if (button0.fallingEdge()) puzzleSolved = true;
  if (button0.risingEdge()) puzzleSolved = false;

  isPuzzleSolved();

  //------------------------ERM-----------------------------------
  // listen for incoming Ethernet connections:
  listenForEthernetClients();

  // Maintain DHCP lease
  if (useDHCP) {
    Ethernet.maintain();
  }

  //--------------------------------------------------------------
}
// Funzione puzzle risolto. PUZNUM è il numero del trigger che controlla Clue Control.
// scrivi all'interno altro codice eventualmente utile
void isPuzzleSolved() {
  if(puzzleSolved){
  //l'imbuto dovrà far partire e fermare un video, quindi ogni momento dovrà aggiornare il sistema. Se disimpegnato, interrompe il video
  Serial.print("UNO se è alto: "); Serial.println(puzzleSolved);
  }
}

// Azione su ricezione comando "trigger" da ERM
void trigger() {
  Serial.println(puzzleSolved);
}

// Resetta il gioco
void reset() {
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

