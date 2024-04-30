#include "SMTPEmailSender.h"
#include "ThingSpeakUploader.h"

// WiFi credentials
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;

// E-mail credentials
const char* senderEmail = "34315FPG15@gmail.com";    // E-mail to send data from
const char* senderPassword = "ppab getq kzoq wyvf";  // App pass for the sender e-mail
const char* receiverEmail = "mkrhimlev@gmail.com";   // E-mail to receive data

// ThingSpeak API Key
const char* writeApiKey = "DJSWCO4HFH9GLVB5";

// ThingSpeak Channel ID
unsigned long channelID = 2460589;

SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);
ThingSpeakUploader tsUploader(ssid, password, writeApiKey, channelID);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.print(".");
  } 
  Serial.println("Connected to Serial.");

  // tsUploader.wifiBegin(); // Not necessary if smtpSender.wifiBegin() is also called

  // SMTP initialization
  smtpSender.wifiBegin();
  smtpSender.SMTPConnect();
  delay(1000);
}

void loop() {
  static String receivedData = ""; // static variable to store incoming data
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (receivedData.length() > 1) { // check data validity
        // Find key and value of data
        char key = receivedData[0];
        String value = receivedData.substring(1);
        
        switch (key) {
          case '1':
            Serial.print("\nTemperature: ");
            Serial.print(value);
            tsUploader.uploadData(1, value.toInt());
            break;
          case '2':
            Serial.print("\nHumidity: ");
            Serial.print(value);
            tsUploader.uploadData(2, value.toInt());
            break;
          case '3':
            Serial.print("\nMoisture: ");
            Serial.print(value);
            tsUploader.uploadData(3, value.toInt());
            break;
          case '4':
            Serial.print("\nSending email now");
            smtpSender.sendEmail("TITEL HELLO WORLD", "SUBJECT TEST", "HELLO WOLRD");
            break;
          default:
            Serial.print("\nReceived: ");
            Serial.print(key);
            Serial.print(" : ");
            Serial.print(value);
            break;
        }
      }
      receivedData = ""; // Reset the buffer for the next message
    } 
    else {
      // Append character to received data
      receivedData += c;
    }
  }
}