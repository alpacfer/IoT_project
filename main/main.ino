#include "IOFunctions.h"
#include "LedControl.h"
#include <Wire.h>
#include <SoftwareSerial.h>

// Define the RX and TX pins for SoftwareSerial
const int rxPin = 3;  // RX pin of ESP8266 connected to pin 3
const int txPin = 4;  // TX pin of ESP8266 connected to pin 4 (TX pin can also be used as RX)

SoftwareSerial esp8266(rxPin, txPin);

// These constants are set up according to the plat:
const int targetMoisture = 40; //Set target soil moisture
const int targetSunlightHours = 5; //Set target daily amount of sunlight
const int targetTemp[2] = {15, 30}; //Set target ambient temperature range
const int targetHum[2] = {15, 50}; //Set target ambient humidity range

const int targetSunlight = targetSunlightHours * 3600; // Converted to seconds, don't touch this

int receivedInt;
int temperature = 0;
int humidity = 0;
int moisture = 0; // Mapped to 0-100
int watering = 0;
int Day;
int old_day;
unsigned long totalLightDuration = 0; // To hold the accumulated duration


// WiFi credentials
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;
int sendEmailCounter = 0; // when to send email
// E-mail credentials
const char* senderEmail = "34315FPG15@gmail.com";    // E-mail to send data from
const char* senderPassword = "ppab getq kzoq wyvf";  // App pass for the sender e-mail
const char* receiverEmail = "l.eilsborg@gmail.com";   // E-mail to receive data

int iterationCounter = 0; // Used as clock divider to show test/demo of emails without spamming

void setup() {
  IOsetup(); //This function handles setup for IO

  Serial.begin(9600);
  esp8266.begin(9600);
  delay(1000); // Wait for serial module inits

  pinMode(8, OUTPUT);

  // start wirecommunication
  Wire.begin(8); /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */

}

void loop() {
  //Run functions to update variables
  readDHT11(&temperature, &humidity);
  updateMoisture(&moisture);
  updateLightDuration(totalLightDuration, millis());

  //Print variables to Serial Monitor
  printVar(temperature, humidity, moisture, totalLightDuration);

  //Perform actions based on variables
  outputControl(temperature, humidity, moisture, targetMoisture, targetTemp, targetHum);

  if (Day != old_day){
    totalLightDuration = 0;
    old_day = Day;
    esp8266.println("4" + 0);   // Trigger the send email status
  }
  // Send variables to Thingspeak via ESP8266
  /* Key with corresponding action
    1 - Push to Temperature Thingspeak channel
    2 - Push to Humidity Thingspeak channel
    3 - Push to Moisture ThingSpeak channel
    4 - Send email
  */
  esp8266.println("1" + String(temperature)); 
  delay(20000);
  esp8266.println("2" + String(humidity));
  delay(20000);
  esp8266.println("3" + String(moisture));
  delay(20000); // Wait a bit before reading again
  if (watering == 1){
    addWater(500);
  }

  iterationCounter++;
  if (iterationCounter >= 2){ // Used for demo and testing 
    esp8266.println("4" + 0);   // Trigger the send email status
    iterationCounter = 0;
  }
}


// function that executes whenever data is received 
void receiveEvent(int howMany) {
 while (1 <Wire.available()) {
  
  byte receivedHighByte = Wire.read();
  byte receivedLowByte = Wire.read();
  receivedInt = (receivedHighByte << 8) | receivedLowByte;
  
    char c = Wire.read();         // receive byte as a character
    //Serial.print(c);              // print the character
  }
  int x = Wire.read();  // receive byte as an integer
  Serial.println(receivedInt);    // print the integer
  if (receivedInt <=100){
      watering = receivedInt;
      Serial.println(watering);
      } else {
        Day = receivedInt-100;
      }
 }


// function that executes whenever data is requested 
void requestEvent() {
  //int myInt = random(0,100); // Example integer value
  int myInt = Day;
  int i;
  byte lowByte = myInt & 0xFF; // Extract LSB
  byte highByte = (myInt >> 8) & 0xFF; // Extract MSB
  Wire.write(highByte);
  Wire.write(lowByte);
  if (i<2){
    int myInt = watering;
  byte lowByte = myInt & 0xFF; // Extract LSB
  byte highByte = (myInt >> 8) & 0xFF; // Extract MSB
  Wire.write(highByte);
  Wire.write(lowByte);
  i = 0;
  }
  i++;
  
}