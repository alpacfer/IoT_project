#include "IOFunctions.h"
#include "LedControl.h"
#include <Wire.h>


// These constants are set up according to the plat:
const int targetMoisture = 40; //Set target soil moisture
const int targetSunlightHours = 5; //Set target daily amount of sunlight
const int targetTemp[2] = {15, 30}; //Set target ambient temperature range
const int targetHum[2] = {15, 40}; //Set target ambient humidity range


const int targetSunlight = targetSunlightHours * 3600; // Converted to seconds, don't touch this

int receivedInt;
int temperature = 0;
int humidity = 0;
int moisture = 0; // Mapped to 0-100
int wartering = 0;
int Day;
unsigned long totalLightDuration = 0; // To hold the accumulated duration


// WiFi credentials
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;


void setup() {
  IOsetup(); //This function handles setup for IO
  Serial.begin(9600);
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
  
  delay(2000); // Wait a bit before reading again
  if (Day == 0){
    // Read moisture from Uno
    Wire.requestFrom(2, 2); /* request & read data of size 2 from Uno */
    if (Wire.available()){
      byte receivedHighByte = Wire.read();
      byte receivedLowByte = Wire.read();
      receivedInt = (receivedHighByte << 8) | receivedLowByte;
      Day = receivedInt;
    }
  }
  
  
  Serial.println(Day);
  
}


// function that executes whenever data is received 
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
  byte receivedHighByte = Wire.read();
  byte receivedLowByte = Wire.read();
  receivedInt = (receivedHighByte << 8) | receivedLowByte;
  if (receivedInt != wartering){
      wartering = !wartering;
      } else {
        Day = receivedInt-100;
      }
  
 }
}

// function that executes whenever data is requested 
void requestEvent() {
  //int myInt = random(0,100); // Example integer value
  int myInt = moisture;
  byte lowByte = myInt & 0xFF; // Extract LSB
  byte highByte = (myInt >> 8) & 0xFF; // Extract MSB
  Wire.write(highByte);
  Wire.write(lowByte);
}