#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

// WiFi credentials
const char* WIFI_SSID = "QuePasa";
const char* WIFI_PASSWORD = "raviolilove";

const char* SMTP_HOST = "smtp.gmail.com";
unsigned int SMTP_PORT = 465;

const char* AUTHOR_EMAIL = "34315FPG15@gmail.com";
const char* AUTHOR_PASSWORD = "ppab getq kzoq wyvf";
const char* RECIPIENT_EMAIL = "mkrhimlev@gmail.com";

SMTPSession smtp;

void setup() {
  Serial.begin(9600);
  delay(1000); // Delay to allow serial connection to be established

  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");

  // smtp.debug(1);

  Serial.println("Configuring SMTP session...");
  Session_Config config; // Structure used in ESP Mail Client library to configure parameters of the SMTP session
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  // config.login.user_domain = "";

  SMTP_Message message; // Structure used in ESP Mail Client library for representing an email message
  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("ESP Test Email");
  message.addRecipient(F("Malthe"), RECIPIENT_EMAIL);

  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();

  Serial.println("Connecting to SMTP server...");
  if (!smtp.connect(&config)) {
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()) {
    Serial.println("\nNot yet logged in.");
  } else {
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  /* Start sending Email and close the session */
  Serial.println("Sending email...");
  if (!MailClient.sendMail(&smtp, &message)) {
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
  }
  Serial.println("Done...");
}

void loop() {
  // Nothing here
}
