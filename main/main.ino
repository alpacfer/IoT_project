#include "IOFunctions.h"
#include "LedControl.h"


// These constants are set up according to the plat:
const int targetMoisture = 40; //Set target soil moisture
const int targetSunlightHours = 5; //Set target daily amount of sunlight
const int targetTemp[2] = {15, 30}; //Set target ambient temperature range
const int targetHum[2] = {15, 40}; //Set target ambient humidity range


const int targetSunlight = targetSunlightHours * 3600; // Converted to seconds, don't touch this

int temperature = 0;
int humidity = 0;
int moisture = 0; // Mapped to 0-100
unsigned long totalLightDuration = 0; // To hold the accumulated duration


// WiFi credentials
const char* ssid = "QuePasa";
const char* password = "raviolilove";

// SMTP credentials
const char* smtpHost = "smtp.gmail.com";
unsigned int smtpPort = 465;

// E-mail credentials
const char* senderEmail = "34315FPG15@gmail.com";    // E-mail to send data from
const char* senderPassword = "ppab getq kzoq wyvf";  // App pass for the sender e-mail
const char* receiverEmail = "l.eilsborg@gmail.com";   // E-mail to receive data

SMTPEmailSender smtpSender(ssid, password, smtpHost, smtpPort, senderEmail, senderPassword, receiverEmail);


void setup() {
  IOsetup(); //This function handles setup for IO
  Serial.begin(9600);
  pinMode(8, OUTPUT);

  // SMTP initialization
  smtpSender.wifiBegin();
  smtpSender.SMTPConnect();
  // test sending of email
  smtpSender.sendEmail("TITEL TEST", "SUBJECT TEST", "TEXT TEXT TEXT HELLO");

}

void loop() {
  //Run functions to update variables
  readDHT11(&temperature, &humidity);
  updateMoisture(&moisture);
  updateLightDuration(totalLightDuration, millis());

  //Print variables to Serial Monitor
  printVar(temperature, humidity, moisture, totalLightDuration);

  
  //Perform actions based on variables
  outputControl(temperature, humidity, moisture, targetMoisture, targetTemp, targetHum);
  
  delay(2000); // Wait a bit before reading again
  
}
