#include "SMTPEmailSender.h"

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

SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);

void setup() {
  Serial.begin(9600);
  delay(1000); // Delay to allow serial connection to be established

  // SMTP initialization
  smtpSender.wifiBegin();
  smtpSender.SMTPConnect();

  // smtpSender.sendEmail("TITEL TEST", "SUBJECT TEST", "TEXT TEXT TEXT HELLO");
}

void loop() {
  delay(60000);
  smtpSender.sendEmail("TITEL TEST", "SUBJECT TEST", "TEXT TEXT TEXT HELLO");
}
