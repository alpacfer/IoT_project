#include <SoftwareSerial.h>

// Define the RX and TX pins for SoftwareSerial
const int rxPin = 2;  // RX pin of ESP8266 connected to pin 2
const int txPin = 3;  // TX pin of ESP8266 connected to pin 3

SoftwareSerial esp8266(rxPin, txPin);

void setup() {
  // Init serial coms
  Serial.begin(9600);
  esp8266.begin(9600);

  // Wait for serial module inits
  delay(1000);
}

void loop() {
  int data = random(0,100);
  // Serial.println(String(data));
  esp8266.println(data);
  delay(1000);
}