#ifndef ThingSpeakUploader_h
#define ThingSpeakUploader_h

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "Arduino.h"
#include <ThingSpeak.h>
#include <WiFiClient.h>
#include "ThingSpeakUploader.h"

class ThingSpeakUploader {
  public:
    ThingSpeakUploader(const char* ssid, const char* password, const char* writeApiKey, unsigned long channelID);
    void wifiBegin();
    bool uploadData(int field, int data);

  private:
    const char* _ssid;
    const char* _password;
    const char* _writeApiKey;

    const char* _server;
    unsigned long _channelID;

    WiFiClient _client;
};

#endif