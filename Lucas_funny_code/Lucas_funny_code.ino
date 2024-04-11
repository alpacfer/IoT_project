#include "LiquidCrystal.h"
#include "Functions.h"
#include "thingProperties.h"
#include "Timer5.h"
LiquidCrystal lcd(4, 10, 9, 8, 7, 6);




void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  Serial.begin(9600); // Serial start to the computer


  // Pin Modes

}




void loop() {

  ArduinoCloud.update();
    // Your code here
    int test = test();
    delay(1000);

}



