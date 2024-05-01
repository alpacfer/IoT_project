
#include <SimpleDHT.h>
#include <LedControl.h>

// for DHT11, 
static const int pinDHT11 = 13;
SimpleDHT11 dht11;

// For Photoresistor
const int photoresistor = A0;

// For Hygrometer
const int hygrometer = A1;

// For Water pump
const int pump = 8;

//MAX7219 stuff
int DIN = 11;
int CS = 12;
int CLK = 13;
LedControl lc=LedControl(DIN, CLK, CS,0);

//Figures for MAX7219
int empty = B00000000;
int full = B11111111;
int Good[8] ={empty,empty,B00100100,empty,B01000010,B00111100,empty,empty}; //All variables are good
int Watering[8] ={empty,B00010000,B00010000,B00111000,B01111100,B01111100,B01111100,B00111000}; //Pumping water
int MoreLight[8] ={B10000001,B01000010,B00011000,B00111100,B00111100,B00011000,B01000010,B10000001}; //Need more sunlight
int LessLight[8] ={B10000001,B01011010,B00100100,B0100010,full,empty,empty,empty}; //Too much sunlight
int TempHigh[8] ={B10000111,B10110001,B10110011,B10110001,B10110111,B10110001,B10110011,B10110001}; //Ambient temperature too high
int TempLow[8] ={B10000111,B10000001,B10000011,B10000001,B10000111,B10000001,B10000011,B10110001}; //Ambient temperature too low
int HumHigh[8] ={empty, B00100010, B00100111, B01110010, B11111010, B11111010, B01110010, empty}; //Ambient humidity too high
int HumLow[8] ={B00100000, B00100010, B01110010, B11111010, B11111010, B01110111, B00000010, empty}; //Ambient humidity too low


//Setup function
void IOsetup(){
  pinMode(A0, INPUT); //Photoresistor
  pinMode(A1, INPUT); //Hygrometer
  pinMode(8, OUTPUT); //Pump

  //MAX7219 stuff
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
}

// Function that outputs ambient temp and humidity using a DHT11
void readDHT11(int *temperature, int *humidity) {
    byte temp, hum;
    if (dht11.read(pinDHT11, &temp, &hum, NULL) == 0) {
        *temperature = (int)temp;
        *humidity = (int)hum;
    }
}


// Function to accumulate the duration light level is above a certain threshold
void updateLightDuration(unsigned long &duration, unsigned long currentTime) {
    static unsigned long lastCheckTime = 0; // When we last checked the light level
    const int threshold = 200; // Light level threshold
    static bool isAboveThreshold = false; // Is light currently above the threshold?
    int lightValue = analogRead(photoresistor); //Read photoresistor
    
    // Initialize lastCheckTime if it's the first run
    if (lastCheckTime == 0) {
        lastCheckTime = currentTime;
    }

    // Check if light is above threshold
    if (lightValue > threshold) {
        if (!isAboveThreshold) {
            // Light just went above the threshold
            isAboveThreshold = true;
        }
        // Accumulate duration since last check as light remains above threshold
        duration += currentTime - lastCheckTime;
    } else {
        // Reset isAboveThreshold if light drops below the threshold
        isAboveThreshold = false;
    }

    // Update lastCheckTime to the current time
    lastCheckTime = currentTime;
}

void LEDMatrixWrite(String command){
  if (command == "Good"){
    for(int i=0;i<8;i++) lc.setRow(0,i,Good[i]);
  }
  else if (command == "Watering"){
    for(int i=0;i<8;i++) lc.setRow(0,i,Watering[i]);
  }
  else if (command == "MoreLight"){
    for(int i=0;i<8;i++) lc.setRow(0,i,MoreLight[i]);
  }
  else if (command == "LessLight"){
    for(int i=0;i<8;i++) lc.setRow(0,i,LessLight[i]);
  }
  else if (command == "TempHigh"){
    for(int i=0;i<8;i++) lc.setRow(0,i,TempHigh[i]);
  }
  else if (command == "TempLow"){
    for(int i=0;i<8;i++) lc.setRow(0,i,TempLow[i]);
  }
  else if (command == "HumHigh"){
    for(int i=0;i<8;i++) lc.setRow(0,i,HumHigh[i]);
  }
  else if (command == "HumLow"){
    for(int i=0;i<8;i++) lc.setRow(0,i,HumLow[i]);
  }
}

//Function to update soil moisture
void updateMoisture(int *moisture){
  *moisture = analogRead(hygrometer);	//Read analog moisture 
	*moisture = constrain(*moisture,400,1023);	//Keep the ranges
	*moisture = map(*moisture,400,1023,100,0);	//Map moisture : 400 will be 100 and 1023 will be 0
}

//Function to add x time of water
void addWater(int time){
  LEDMatrixWrite("Watering"); //Indicate action
  digitalWrite(pump, HIGH); //Pump water
  delay(time);
  digitalWrite(pump, LOW); // Stop pump
}

//Function to print variables to serial monitor in a readable format, mainly for debugging purposes
void printVar(int temperature, int humidity, int moisture, unsigned long totalLightDuration){
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Moisture: " );
  Serial.print(moisture);

  // Convert milliseconds to hours, minutes, and seconds for display
  unsigned long hours = (totalLightDuration / (1000 * 60 * 60));
  unsigned long mins = (totalLightDuration / (1000 * 60)) % 60;
  unsigned long secs = (totalLightDuration / 1000) % 60; 
  Serial.print(", Sunlight: ");
  Serial.print(hours);
  Serial.print("h ");
  Serial.print(mins);
  Serial.print("m ");
  Serial.print(secs);
  Serial.println("s");
}

//Function that takes in all variables and their respective targets and performs according function
void outputControl(int temperature, int humidity, int moisture,  int targetMoisture, int targetTemp[2], int targetHum[2]){
  if (moisture < targetMoisture){ //water dosing control is handled first
    addWater(300); //This time should be adjusted according to the vol
  } else if (temperature < targetTemp[0]){ //Next priority is temperature
    LEDMatrixWrite("TempLow");
  } else if (temperature > targetTemp[1]){
    LEDMatrixWrite("TempHigh");
  }  else if (humidity < targetHum[0]){ //Ambient humidity
    LEDMatrixWrite("HumLow");
  } else if (humidity > targetHum[1]){
    LEDMatrixWrite("HumHigh");
  } else{ // All variables are withing specification
    LEDMatrixWrite("Good");
  }
}
