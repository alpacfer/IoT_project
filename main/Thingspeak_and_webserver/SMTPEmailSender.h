#ifndef SMTPEmailSender_h
#define SMTPEmailSender_h

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
#include "SMTPEmailSender.h"

class SMTPEmailSender {
  public:
    SMTPEmailSender(
      const char* ssid, 
      const char* password, 
      const char* smtpHost, 
      unsigned int smtpPort,
      const char* senderEmail,
      const char* senderPassword,
      const char* receiverEmail
    );
    void wifiBegin();
    void SMTPConnect();
    void sendEmail(const char* title, const char* subject,const char* text);

  private:
    const char* _ssid;
    const char* _password;

    const char* _smtpHost;
    unsigned int _smtpPort;

    const char* _senderEmail;
    const char* _senderPassword;
    const char* _receiverEmail;

    SMTPSession smtp;
    Session_Config config;  // Structure used in ESP_Mail_Client library to configure parameters of the SMTP session
};

#endif