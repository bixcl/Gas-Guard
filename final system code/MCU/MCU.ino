#define BLYNK_TEMPLATE_ID "XXX"
#define BLYNK_TEMPLATE_NAME "XXX"
#define BLYNK_AUTH_TOKEN "XXX"
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

int OW = 80;

char ssid[] = "XXX";
char pass[] = "XXX";

BlynkTimer timer;

SoftwareSerial nodemcu(D2, D3);
void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  //while (!Serial) continue;
}

void loop() {
  if ( nodemcu.available() > 0){
    int load = nodemcu.read();
    //int LoadPercent = (load/OW) * 100;
    //Serial.println(LoadPercent);
    Serial.println(load);
    //Blynk.virtualWrite(V0,LoadPercent);
    Blynk.virtualWrite(V0,load);
    if (load < 20 and load > 10){
      Blynk.logEvent("gas_alert","Gas percentage less than 20%");
      Blynk.logEvent("gas_email","my gas is about to finish, i need a new one");
      delay(1000);
    }
    
  }
  
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  delay(1000);
  

}