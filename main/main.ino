#include "IOFunctions.h"
#include "LedControl.h"
#include <SoftwareSerial.h>

// Define the RX and TX pins for SoftwareSerial
const int rxPin = 3;  // RX pin of ESP8266 connected to pin 3
const int txPin = 4;  // TX pin of ESP8266 connected to pin 4 (TX pin can also be used as RX)

SoftwareSerial esp8266(rxPin, txPin);

// These constants are set up according to the plat:
const int targetMoisture = 40; //Set target soil moisture
const int targetSunlightHours = 5; //Set target daily amount of sunlight
const int targetTemp[2] = {15, 30}; //Set target ambient temperature range
const int targetHum[2] = {15, 40}; //Set target ambient humidity range

const int targetSunlight = targetSunlightHours * 3600; // Converted to seconds, don't touch this

int temperature = 0;
int humidity = 0;
int moisture = 0; // Mapped to 0-100
unsigned long totalLightDuration = 0; // To hold the accumulated duration

int sendEmailCounter = 0; // when to send email

void setup() {
  IOsetup(); //This function handles setup for IO

  Serial.begin(9600);
  esp8266.begin(9600);
  delay(1000); // Wait for serial module inits

  pinMode(8, OUTPUT);
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

  //Send variables to Thingspeak
  // esp8266.println("1" + String(temperature));
  // delay(5000);
  // esp8266.println("2" + String(humidity));
  // delay(5000);
  // esp8266.println("3" + String(moisture));
  // delay(5000); // Wait a bit before reading again
  
  if (sendEmailCounter > 1){
    esp8266.println("4" + String(0));
    sendEmailCounter = 0;
    Serial.println("SENDING EMAIL");
    delay(2000);
  }

  sendEmailCounter++;
}
