#include "SMTPEmailSender.h"
#include "ThingSpeakUploader.h"
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WiFi.h>
#include <Wire.h> // library for i2c communication
#include <time.h>

// Replace with your network details
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;

// E-mail credentials
const char* senderEmail = "34315FPG15@gmail.com";    // E-mail to send data from
const char* senderPassword = "ppab getq kzoq wyvf";  // App pass for the sender e-mail
const char* receiverEmail = "mkrhimlev@gmail.com";   // E-mail to receive data

// ThingsSpeak credentials
unsigned long channelID = 2029121; //your channel
const char * myWriteAPIKey = "EOEPEW3V0RKNBQZE"; // your WRITE API key
const char* server_api = "api.thingspeak.com";


WiFiClient client;

ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);
// Initialize the SMTP client
SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);
// Instance of ThingSpeakUploader
ThingSpeakUploader tsUploader(ssid, password, myWriteAPIKey, channelID);
// Initialize WebPage functions
void handleRoot();  
void handleLED();  
void handleNotFound();

//global varibles
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;                              // the structure tm holds time information in a more convenient way
int lastTemperature = 0;
int lastHumidity = 0;
int lastMoisture = 0;
int receivedInt;
int day = tm.tm_mday;
int Wartering;
int day_check;
int i;
int wartering_old = 0;


void setup() {
  Serial.begin(9600);
  delay(500);
  while (!Serial) {
    Serial.print(".");
  } 
  Serial.println("Connected to Serial.");
  delay(500);

  // tsUploader.wifiBegin(); // Not necessary if smtpSender.wifiBegin() is also called

  // SMTP initialization
  smtpSender.wifiBegin();
  delay(500);
  smtpSender.SMTPConnect();
  delay(500);
  if (MDNS.begin("iot")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_POST, handleLED);
  server.onNotFound(handleNotFound);
    
  // Start the server
  server.begin();
  Serial.println("Server started"); 

  // connecting wire
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NODEMCU */
  // Transmitting day to Uno
  Serial.println("Transmitting day to uno and checking status");
  int Wire_day = 100+day;
  //checking if synch have been made
  while(day_check != day){
    Wire.beginTransmission(8); /* begin with device address 8 */
    byte lowByte = Wire_day & 0xFF; // Extract LSB
    byte highByte = (Wire_day >> 8) & 0xFF; // Extract MSB
    Wire.write(highByte);
    Wire.write(lowByte);
    Wire.endTransmission(); /* stop transmitting */
    Wire.requestFrom(8, 2); /* request & read data of size 13 from slave */
    while(Wire.available()){
    byte receivedHighByte = Wire.read();
    byte receivedLowByte = Wire.read();
    day_check = (receivedHighByte << 8) | receivedLowByte;
    Serial.print(".");
    delay(200);
    if (i==100){
      Serial.println("Couldn't sync date....");
      i=0;
      break;
    }
    i++;
    }
  }




}

void loop() {
  day = tm.tm_mday;
  int Wire_day = 100+day;
  while(day_check != day){
    Wire.beginTransmission(8); /* begin with device address 8 */
    byte lowByte = Wire_day & 0xFF; // Extract LSB
    byte highByte = (Wire_day >> 8) & 0xFF; // Extract MSB
    Wire.write(highByte);
    Wire.write(lowByte);
    Wire.endTransmission(); /* stop transmitting */
    Wire.requestFrom(8, 2); /* request & read data of size 13 from slave */
    while(Wire.available()){
    byte receivedHighByte = Wire.read();
    byte receivedLowByte = Wire.read();
    day_check = (receivedHighByte << 8) | receivedLowByte;
    Serial.print(".");
    delay(200);
    if (i==100){
      Serial.println("Couldn't sync date....");
      i=0;
      break;
    }
    i++;
    Wire.requestFrom(8, 2);
    }
  }
  static String receivedData = ""; // static variable to store incoming data
  while (Serial.available()) {
    // Check if a client has connected
    server.handleClient();
    char c = Serial.read();
    if (c == '\n') {
      if (receivedData.length() > 1) { // check data validity
        // Find key and value of data
        char key = receivedData[0];
        String value = receivedData.substring(1);
        switch (key) {
          case '1':
            Serial.print("\nTemperature: ");
            Serial.print(value);
            lastTemperature = value.toInt();
            tsUploader.uploadData(1, lastTemperature);
            break;
          case '2':
            Serial.print("\nHumidity: ");
            Serial.print(value);
            lastHumidity = value.toInt();
            tsUploader.uploadData(2, lastHumidity);
            break;
          case '3':
            Serial.print("\nMoisture: ");
            Serial.print(value);
            lastMoisture = value.toInt();
            tsUploader.uploadData(3, lastMoisture);
            break;
          case '4':
            char title[100]; // Adjust the size as needed
            sprintf(title, "Temperature: %d C, Humidity: %d, Moisture: %d", lastTemperature, lastHumidity, lastMoisture);
            smtpSender.sendEmail("Demo", "Current plant status", title);
            break;
          default:
            Serial.print("\nReceived: ");
            Serial.print(key);
            Serial.print(" : ");
            Serial.print(value);
            break;
        }
      }
      delay(5000);
      receivedData = ""; // Reset the buffer for the next message
    } 
    else {
      // Append character to received data
      receivedData += c;
    }
  }
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Internet of Things - Group 15: Smart Plant Pot </title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Welcome to the plant wartering webclient</h1> \ .</p> \
      <p>Click the buttom below to turn on or off the pump, please wait at least 2 secounds between pushing it</p> \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Turn on or Off warter pump\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
}

void handleLED() {                          // If a POST request is made to URI /LED
  Wartering = !Wartering;
  // Sending the warter pump status to UNO
  if(wartering_old != Wartering){
    Wire.beginTransmission(8); /* begin with device address 8 */
    byte lowByte = Wartering & 0xFF; // Extract LSB
    byte highByte = (Wartering >> 8) & 0xFF; // Extract MSB
    Wire.write(highByte);
    Wire.write(lowByte);
    Wire.endTransmission(); /* stop transmitting */
    Serial.print("LED");
    Serial.println(Wartering);
    while(Wire.available()){
    Wire.requestFrom(8, 2);
    byte receivedHighByte = Wire.read();
    byte receivedLowByte = Wire.read();
    wartering_old = (receivedHighByte << 8) | receivedLowByte;
    Serial.print(".");
    delay(200);
    if (i==100){
      Serial.println("Couldn't sync date....");
      i=0;
      break;
    }
    i++;
    }
    }                            // Turn on watering
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303); // Send it back to the browser with an HTTP status 303 (See Other) to redirect
          
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}