// Including the ESP8266 WiFi library
#include "ThingSpeakUploader.h"
#include <ESP8266mDNS.h> 

ThingSpeakUploader::ThingSpeakUploader(const char* ssid, const char* password, const char* writeApiKey, unsigned long channelID) {
  /*
  Constructor containing wifi name (ssid), password, write key api and channel id to allow uploading of data
  */
  _ssid = ssid;
  _password = password;
  _writeApiKey = writeApiKey;

  _channelID = channelID;
  _server = "api.thingspeak.com";
}

void ThingSpeakUploader::wifiBegin(){
  /*
  Connect to WiFi
  */
  WiFi.begin(_ssid, _password);
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Failed to connect to WiFi");
  }
  Serial.println("Wifi Connected");
  delay(100);
  Serial.println("Server IP Address: ");
  delay(100);
  Serial.println(WiFi.localIP());
  delay(100);

  
}

bool ThingSpeakUploader::uploadData(int field, int data) {
  /*
  Upload datapoint to field on ThingSpeak...
  Inputs:
  - int field: Integer value for field to put datapoint
  - int data: Integer datapoint value to upload to ThingSpeak field

  Outputs:
  - bool: returns false if connection to ThingSpeak fails
  */

  ThingSpeak.begin(_client);
  if (!_client.connect(_server, 80)) {
    Serial.println("Failed to connect to ThingSpeak");
    return false;
  }

  ThingSpeak.setField(field, data);

  ThingSpeak.writeFields(_channelID, _writeApiKey);

  _client.stop();
  Serial.println("   Uploading " + String(data) + " to field " + String(field));
  return true;
}