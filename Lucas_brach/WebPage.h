#ifndef WebPage_h
#define WebPage_h

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
#include "Arduino.h"

class WebPage {
  public:
    WebPage(const int* test);
    void handleRoot();
    void handleLED(int led);
    void handleNotFound();

  private:
    const int* _test;
    //const char* _password;
    //const char* _writeApiKey;

    //const char* _server;
    //unsigned long _channelID;
};

#endif