void setup() {
  Serial.begin(9600);
  while (!Serial) {
    Serial.print(".");
  } 
  Serial.println("Connected to Serial.");
}

void loop() {
  if (Serial.available()) {
    // Read incoming byte from uno
    char receivedChar = Serial.read();
    Serial.print(receivedChar);
  }
}