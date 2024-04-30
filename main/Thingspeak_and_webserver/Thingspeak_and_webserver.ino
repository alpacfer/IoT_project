// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include "SMTPEmailSender.h"
#include "ThingSpeakUploader.h"
#include <Wire.h> // library for i2c communication
#include <time.h>


// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
ESP8266WiFiMulti wifiMulti;


int Wartering = 0;
int i = 0;
const int DHTPin = D3;
int dirt_moisture = 0;
int receivedInt;
int day;


// Replace with your network details
const char* ssid = "Lucas";
const char* password = "123456789abc";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;

// E-mail credentials
const char* senderEmail = "34315FPG15@gmail.com";    // E-mail to send data from
const char* senderPassword = "ppab getq kzoq wyvf";  // App pass for the sender e-mail
const char* receiverEmail = "l.eilsborg@gmail.com";   // E-mail to receive data

// ThingsSpeak credentials
unsigned long channelID = 2029121; //your channel
const char * myWriteAPIKey = "EOEPEW3V0RKNBQZE"; // your WRITE API key
const char* server_api = "api.thingspeak.com";

const int postingInterval = 20 * 1000; // post data every 20 seconds


//Time global varibles
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;                              // the structure tm holds time information in a more convenient way



WiFiClient client;


// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];





// Create an instance of the server
ESP8266WebServer server(80);
// Initialize the SMTP client
SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
// Instance of ThingSpeakUploader
ThingSpeakUploader tsUploader(ssid, password, myWriteAPIKey, channelID);
// Initialize WebPage functions
void handleRoot();  
void handleLED();  
void handleNotFound();

// only runs once on boot
void setup() {
  delay(5000);
  // Initializing serial port for debugging purposes
  Serial.begin(9600);
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NODEMCU */
  // Wait for serial to initialize. This is used to conect to the UNO
  while (!Serial) {
    Serial.print(".");
  } 
  Serial.println("Connected to Serial.");
  // getting the current day
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
  day = tm.tm_mday;




  dht.begin();
  
  tsUploader.wifiBegin();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  delay(100);
  Serial.println("Server IP Address: ");
  delay(100);
  Serial.println(WiFi.localIP());
  delay(100);

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


   // SMTP initialization
  smtpSender.SMTPConnect();

  
}

// runs over and over again
void loop() {
  // Check if a client has connected
  server.handleClient();
  Serial.println(WiFi.localIP());

  // **** This part reads only sensors and calculates
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (day !=tm.tm_mday){
    day = tm.tm_mday;
    Serial.print(day);
    int Wire_day = 100+day;
    Wire.beginTransmission(8); /* begin with device address 8 */
    byte lowByte = Wire_day & 0xFF; // Extract LSB
    byte highByte = (Wire_day >> 8) & 0xFF; // Extract MSB
    Wire.write(highByte);
    Wire.write(lowByte);
    Wire.endTransmission(); /* stop transmitting */
  }

  tsUploader.uploadData(1,t);

  // Sending the warter pump status to UNO
  Wire.beginTransmission(8); /* begin with device address 8 */
  byte lowByte = Wartering & 0xFF; // Extract LSB
  byte highByte = (Wartering >> 8) & 0xFF; // Extract MSB
  Wire.write(highByte);
  Wire.write(lowByte);
  Wire.endTransmission(); /* stop transmitting */

  

  // Read moisture from Uno
  Wire.requestFrom(8, 2); /* request & read data of size 2 from Uno */
  
 if (Wire.available()){
  byte receivedHighByte = Wire.read();
  byte receivedLowByte = Wire.read();
  receivedInt = (receivedHighByte << 8) | receivedLowByte;
 }
  


  tsUploader.uploadData(6,receivedInt);
  Serial.print("day: ");
  Serial.println(day);
  // Send an e-mail
  if (receivedInt < 20) {
    smtpSender.sendEmail("Plant updater", "Update on plant", "Hallo your plant is thirsty, the moisturelevel is:  please water it.  Click this link if you're home to water the plant:  ");
  }
  delay(8000);
}



void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Internet of Things - Group 15: Smart Plant Pot </title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Welcome to the plant wartering webclient</h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      </body></html>");
}

void handleLED() {                          // If a POST request is made to URI /LED
  Wartering = !Wartering;                            // Turn on watering
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303); // Send it back to the browser with an HTTP status 303 (See Other) to redirect
          
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}