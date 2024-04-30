
#include <Wire.h>
int receivedInt;
void setup() {
 Wire.begin(8); /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600); /* start serial for debug */
}

void loop() {
 delay(100);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
  byte receivedHighByte = Wire.read();
  byte receivedLowByte = Wire.read();
  receivedInt = (receivedHighByte << 8) | receivedLowByte;
  Serial.print(receivedInt);
 }
 Serial.println(); /* to newline */
}

// function that executes whenever data is requested from master
void requestEvent() {
  int myInt = random(0,100); // Example integer value
  Serial.println(myInt);
  byte lowByte = myInt & 0xFF; // Extract LSB
  byte highByte = (myInt >> 8) & 0xFF; // Extract MSB
  Wire.write(highByte);
  Wire.write(lowByte);
}
