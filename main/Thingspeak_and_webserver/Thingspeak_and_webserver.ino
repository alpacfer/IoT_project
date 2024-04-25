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
#include "WebPage.h"





#define DHTTYPE DHT11   // DHT 11

ESP8266WiFiMulti wifiMulti;
int led = 0;
int i = 0;


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




WiFiClient client;

const int DHTPin = D3;


// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

// ThingSpeak Channel ID
unsigned long channelID = 2029121; //your channel
const char * myWriteAPIKey = "EOEPEW3V0RKNBQZE"; // your WRITE API key


const int postingInterval = 20 * 1000; // post data every 20 seconds

// Create an instance of the server
ESP8266WebServer server(80);
// Initialize the SMTP client
SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
// Instance of ThingSpeakUploader
ThingSpeakUploader tsUploader(ssid, password, myWriteAPIKey channelID);
// Initialize WebPage
WebPage webPage(led);

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();
  
  // Connecting to WiFi network and return local ip
  tsUploader.wifiBegin();


  server.on("/", HTTP_GET, webPage.handleRoot());
  server.on("/LED", HTTP_POST, webPage.handleLED(led));
  server.onNotFound(webPage.handleNotFound());
    
  // Start the server
  server.begin();
  Serial.println("Server started"); 


   // SMTP initialization
  smtpSender.SMTPConnect();

  
}

// runs over and over again
void loop() {
  // **** This part reads only sensors and calculates
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
        float f = dht.readTemperature(true);

        float hif = 0;
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
          Serial.println("Failed to read from DHT sensor!");
          strcpy(celsiusTemp,"Failed");
          strcpy(fahrenheitTemp, "Failed");
          strcpy(humidityTemp, "Failed");         
        }
        else{
          // Computes temperature values in Celsius + Fahrenheit and Humidity
          float hic = dht.computeHeatIndex(t, h, false);       
          dtostrf(hic, 6, 2, celsiusTemp);             
          float hif = dht.computeHeatIndex(f, h);
          dtostrf(hif, 6, 2, fahrenheitTemp);         
          dtostrf(h, 6, 2, humidityTemp);

          // You can delete the following Serial.print's, it's just for debugging purposes
          Serial.print("Humidity: ");
          Serial.print(h);
          Serial.print(" %\t Temperature: ");
          Serial.print(t);
          Serial.print(" *C ");
          Serial.print(f);
          Serial.print(" *F\t Heat index: ");
          Serial.print(hic);
          Serial.print(" *C ");
          Serial.print(hif);
          Serial.print(" *F");
          Serial.print("Humidity: ");
          Serial.print(h);
          Serial.print(" %\t Temperature: ");
          Serial.print(t);
          Serial.print(" *C ");
          Serial.print(f);
          Serial.print(" *F\t Heat index: ");
          Serial.print(hic);
          Serial.print(" *C ");
          Serial.print(hif);
          Serial.println(" *F");
    
        }
        tsUploader.uploadData(1, t);
        tsUploader.uploadData(2, hif);
        tsUploader.uploadData(3, h);
        
  delay(60000);
  smtpSender.sendEmail("TITEL TEST", "SUBJECT TEST", "TEXT TEXT TEXT HELLO");
  // wait and then post again
  delay(postingInterval);
}

