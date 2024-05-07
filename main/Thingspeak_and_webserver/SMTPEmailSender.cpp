#include "SMTPEmailSender.h"

SMTPEmailSender::SMTPEmailSender(
    const char* ssid, 
    const char* password, 
    const char* smtpHost,
    unsigned int smtpPort,
    const char* senderEmail,
    const char* senderPassword,
    const char* receiverEmail
  ) {
  /*
  Constructor containing wifi name (ssid), wifi password, smtp host/port, sender/receiver email and sender app password
  */
  _ssid = ssid;
  _password = password;

  _smtpHost = smtpHost;
  _smtpPort = smtpPort;

  _senderEmail = senderEmail;
  _senderPassword = senderPassword;
  _receiverEmail = receiverEmail;
}

void SMTPEmailSender::wifiBegin(){
  /*
  Connect to WiFi
  */
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");
  Serial.println("Server IP Address: ");
  delay(100);
  Serial.println(WiFi.localIP());
  delay(100);

}

void SMTPEmailSender::SMTPConnect(){
  // SMTP credentials
  Serial.println("Configuring SMTP session...");
  config.server.host_name = _smtpHost;
  config.server.port = _smtpPort;
  config.login.email = _senderEmail;
  config.login.password = _senderPassword;

   // Connect to SMTP server
  Serial.println("Connecting to SMTP server...");
  while (!smtp.connect(&config)) {  // Will keep trying to connect to the SMTP if it fails. Sometimes the ESP8266 needs a reset.
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    Serial.println("");
    delay(150);
  }

  if (!smtp.isLoggedIn()) {   // Prints the login state - mostly used for debugging
    Serial.println("\nNot yet logged in.");
  } else {
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }
}

void SMTPEmailSender::sendEmail(const char* title, const char* subject,const char* text){
  SMTP_Message message; // Structure used in ESP Mail Client library for representing an email message
  message.sender.name = title;
  message.sender.email = _senderEmail;
  message.subject = subject;
  message.addRecipient("", _receiverEmail);
  // message.text.content = text.c_str();   // Used String instead of const char*
  message.text.content = text;

  Serial.println("Sending email...");
  if (!MailClient.sendMail(&smtp, &message, false)) {   // false = dont end session
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
  }

  Serial.println("Email has been sent...");
}


