#include "ThingSpeakUploader.h"

// WiFi credentials
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// ThingSpeak API Key
const char* writeApiKey = "DJSWCO4HFH9GLVB5";

// ThingSpeak Channel ID
unsigned long channelID = 2460589;

// Instance of ThingSpeakUploader
ThingSpeakUploader tsUploader(ssid, password, writeApiKey, channelID);

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Starting...");

  tsUploader.wifiBegin();
}

void loop() {
  int data = random(0, 100);
  tsUploader.uploadData(6, data);
  delay(2000);
}

