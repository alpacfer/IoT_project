#include <Wire.h> // library for i2c communication

void setup() {
 Serial.begin(9600); /* begin serial for debug */
 Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NODEMCU */
}

void loop() {
 Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("Hello Arduino"); /* sends hello string */
 Wire.endTransmission(); /* stop transmitting */

 Wire.requestFrom(8, 2); /* request & read data of size 13 from slave */
 while(Wire.available()){
  byte receivedHighByte = Wire.read();
  byte receivedLowByte = Wire.read();
  int receivedInt = (receivedHighByte << 8) | receivedLowByte;
 char c = Wire.read();
 Serial.print(receivedInt);
 }
 Serial.println();
 delay(1000);
}