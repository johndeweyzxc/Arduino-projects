#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial NodeMCU(D1, D2, D3);

// Enter your wifi network name and Wifi Password
const char* ssid = "Redmi 9C NFC";
const char* password = "redmi0507";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// These variables store current output state of LED
String outputRedState = "off";
String outputGreenState = "off";
String outputYellowState = "off";

// Assign output variables to GPIO pins

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);
  NodeMCU.begin(4800);
  // Initialize the output variables as outputs
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  // Set outputs to LOW
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);

  NodeMCU.println("Testing...");
  // Connect to Wi-Fi network with SSID and password
  NodeMCU.println("Connecting to ");
  NodeMCU.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  delay(100);
}

void loop(){
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis(); 
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          Serial.println("HTTP/1.1 200 OK");
          Serial.println("Content-type:text/html");
          Serial.println("Connection: close");
          Serial.println();

          // turns the GPIOs on and off
          if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("RED LED is on");
              outputRedState = "on";
              digitalWrite(D1, HIGH);
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("RED LED is off");
              outputRedState = "off";
              digitalWrite(D1, LOW);
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("YELLOW LED is on");
              outputYellowState = "on";
              digitalWrite(D2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("YELLOW LED is off");
              outputYellowState = "off";
              digitalWrite(D2, LOW);
            } else if (header.indexOf("GET /3/on") >= 0) {
              Serial.println("GREEN LED is on");
              outputGreenState = "on";
              digitalWrite(D3, HIGH);
            } else if (header.indexOf("GET /3/off") >= 0) {
              Serial.println("GREED LED is off");
              outputGreenState = "off";
              digitalWrite(D3, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonRed { background-color: #ff0000; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonGreen { background-color: #00ff00; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonYellow { background-color: #feeb36; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonOff { background-color: #77878A; border: none; color: white; padding: 16px 40px; border-radius: 70%;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>My LED Control Server</h1>");

            // Display current state, and ON/OFF buttons for GPIO 1 Red LED 
            client.println("<p>Red LED is " + outputRedState + "</p>");
            // If the outputRedState is off, it displays the OFF button 
            if (outputRedState=="off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button buttonOff\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button buttonRed\">ON</button></a></p>");
            } 

            // Display current state, and ON/OFF buttons for GPIO 3 Yellow LED 
            client.println("<p>Yellow LED is " + outputYellowState + "</p>");
            // If the outputYellowState is off, it displays the OFF button 
            if (outputYellowState =="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button buttonOff\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button buttonYellow\">ON</button></a></p>");
            }
            client.println("</body></html>");

            // Display current state, and ON/OFF buttons for GPIO 2 Green LED 
            client.println("<p>Green LED is " + outputGreenState + "</p>");
            // If the outputGreenState is off, it displays the OFF button 
            if (outputGreenState =="off") {
              client.println("<p><a href=\"/3/on\"><button class=\"button buttonOff\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/3/off\"><button class=\"button buttonGreen\">ON</button></a></p>");
            }
              client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
        currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
