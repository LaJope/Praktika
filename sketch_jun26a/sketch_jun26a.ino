// Load Wi-Fi library
#include <WiFi.h>

// Network credentials Here
const char* ssid     = "Vova_loh";
const char* password = "dadadada";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  
  WiFi.softAP(ssid,password);
  
  // Print IP address and start web server
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      client.println(
        "<!DOCTYPE html>"
        "<html>"
        " <head>"
        "   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "   <link rel=\"icon\" href=\"data;,\">"
        "   <style>"
        "     html {"
        "       font-famyly: monospace;"
        "       display: inline-block;"
        "       margin: 0px auto;"
        "       text-align: center;"
        "     }"
        "   </style>"
        "  </head>"
        "<body>"
        " <h1>ESP32 Web Server</h1>"
        " <h2>Vova LOH</h2>"
        "</body>"
        "</html>"
        );
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}