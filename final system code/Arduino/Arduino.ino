
#include <Servo.h>
#include <HX711_ADC.h>
#include <SoftwareSerial.h> //connect arduino to MCU//
#include <ArduinoJson.h> //connect arduino to MCU//

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(2, 3); //connect arduino to MCU//

//servo Opem/Close
const int openServo = 90;
const int closeServo = 0;

//load values
const int lessValue = 2;
const int midValue = 26;
const int highValue = 50;

//HX711 pins:
const int HX711_dout = 4; // mcu > HX711 dout pin // load
const int HX711_sck = 5; // mcu > HX711 sck pin // load

//light pins
const int yelloPin = 8;
const int redPin = 9;
const int greenPin = 10;

//kitlight
const int KitRedPin = 11;

// buzzer pin
const int buzzerPin = 7;

const int OutGasPin = A0;  // Analog pin connected to MQ-2 output // Outgas

const int KitGasPin = A2;  // Analog pin connected to MQ-2 output // Kitgas

const int baselineValue = 0; // Adjust this after initial calibration //gas
const int GASthreshold = 600;   // Adjust this threshold based on desired sensitivity //gas

const int OutFlamePin = A1; // Outflame
const int KitFlamePin = A3; // Kitflame

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell(HX711_dout, HX711_sck); // load

const int calVal_eepromAdress = 0; // eeprom address for calibration value (4 bytes) // load
long t; // load


void setup() {
  // Debug console
  Serial.begin(9600);
  nodemcu.begin(9600);//connect arduino to MCU//

  //buzzer
  pinMode(buzzerPin,OUTPUT);

  //lights
  pinMode(yelloPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(KitRedPin, OUTPUT);

  // load
  delay(10); // load
  Serial.println(); // load
  Serial.println("Starting..."); // load
  float calibrationValue; // calibration value // load
  calibrationValue = 696.0; // Uncomment and set this value if you want to set it in the sketch // load
  LoadCell.begin(); // load
  long stabilizingtime = 2000; // Tare precision can be improved by adding a few seconds of stabilizing time // load
  boolean tare = true; //set this to false if you don't want tare to be performed in the next step // load
  byte loadcell_rdy = 0; // load
  while (!loadcell_rdy) {
      loadcell_rdy = LoadCell.startMultiple(stabilizingtime, tare);
  }
  if (LoadCell.getTareTimeoutFlag()) {
      Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  }
  LoadCell.setCalFactor(calibrationValue); // Set calibration value (float)
  Serial.println("Startup is complete");

  // flame
  pinMode(OutFlamePin,INPUT); // flame
  pinMode(KitFlamePin,INPUT); // flame

}

void loop() {
  noTone(buzzerPin);
  digitalWrite(KitRedPin, LOW);

  static boolean newDataReady = 0; // load
  const int serialPrintInterval = 0; // Increase value to slow down serial print activity // load
  // Check for new data/start next conversion: // load
  if (LoadCell.update()) newDataReady = true; //load
  // Get smoothed value from data set //load
  float weight;
  if (newDataReady) { //load
      if (millis() > t + serialPrintInterval) { // load
          weight = LoadCell.getData(); // load
          Serial.print("Load cell output value: "); // load
          Serial.println(weight); // load
          nodemcu.write(weight);
          newDataReady = 0; // load
          t = millis(); // load
      }
  }
  // Receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) { // load
      char inByte = Serial.read(); // load
      if (inByte == 't') { // load
          LoadCell.tareNoDelay(); // load
      }
  }
  // Check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) { // load
      Serial.println("Tare complete"); // load
  }

  // flame
  int thershold=200; // flame
  int OutFlameSensorVal = analogRead(OutFlamePin); // flame
  int KitFlameSensorVal = analogRead(KitFlamePin); // flame

  // gas
  int OutGaSensorVal = analogRead(OutGasPin); //gas
  int KitGaSensorVal = analogRead(KitGasPin); //gas

  Serial.print("\n\n");

  // condition of load sensor.
  if (weight <= midValue and weight > lessValue){
    digitalWrite(redPin, HIGH);
    digitalWrite(yelloPin, LOW);
    digitalWrite(greenPin, LOW);

  }else if (weight <= highValue and weight > midValue) {
    digitalWrite(redPin, LOW);
    digitalWrite(yelloPin, HIGH);
    digitalWrite(greenPin, LOW);
    

  }else if (weight > highValue){
    digitalWrite(redPin, LOW);
    digitalWrite(yelloPin, LOW);
    digitalWrite(greenPin, HIGH);

  }else if (weight <= lessValue){
    digitalWrite(redPin, LOW);
    digitalWrite(yelloPin, LOW);
    digitalWrite(greenPin, LOW);
  
  }

  // condition for outside gas sensor
  if (OutGaSensorVal > 750) { 
    Serial.println("Gas detected in Gas Room!");
    tone(buzzerPin, 272);
    delay(200);
    digitalWrite(KitRedPin, HIGH);
    delay(200);
    digitalWrite(KitRedPin, LOW);
  } 

  // condition for kit gas sensor
  if (KitGaSensorVal > 750) { 
    Serial.println("Gas detected in kitchen!");
    tone(buzzerPin, 272);
    delay(200);
    digitalWrite(KitRedPin, HIGH);
    delay(200);
    digitalWrite(KitRedPin, LOW);
  } 

  Serial.println("kitchen flame: ");
  Serial.println(KitFlameSensorVal);

  Serial.println("Gas Room flame: ");
  Serial.println(OutFlameSensorVal);

  // condition for gas sensor
  if (KitFlameSensorVal <= 100) { 
    Serial.println("Flame detected in kitchen!");
    tone(buzzerPin, 272);
    digitalWrite(KitRedPin, LOW);
    delay(200);
    digitalWrite(KitRedPin, HIGH);
    delay(200);
  } 

  if (OutFlameSensorVal <= 100) { 
  Serial.println("Flame detected in Gas Room!");
  //digitalWrite(buzzerPin, HIGH);
  tone(buzzerPin, 272);
  digitalWrite(KitRedPin, LOW);
  delay(200);
  digitalWrite(KitRedPin, HIGH);
  delay(200);
  }
  //nodemcu.write(weight);
// gas
  delay(1000); // Read sensor value every 500 milliseconds
}