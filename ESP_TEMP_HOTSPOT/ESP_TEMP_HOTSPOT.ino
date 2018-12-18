#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "dht.h"


dht DHT;

/************ WIFI and MQTT Information (CHANGE THESE FOR YOUR SETUP) ******************/
#define OTApassword "yourOTApassword" //the password you will need to enter to upload remotely via the ArduinoIDE
int OTAport = 8266;


// D5==14
#define SENSOR_PIN   14

long lastUpdate = 0;
const long timeBetweenUpdates = 10000;

char ssid[32];
const char* password = "dummyPassword";

/********************************** START SETUP*****************************************/
void setup() {
  Serial.begin(115200);

  //OTA SETUP
  ArduinoOTA.setPort(OTAport);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("TemperatureDemo");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)OTApassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println("Ready");
  sprintf(ssid, "Temperature_xx");
  Serial.println(ssid);
  WiFi.softAP(ssid, password); //begin WiFi access point

}



/********************************** START MAIN LOOP*****************************************/
void loop() {

  ArduinoOTA.handle();

  checkTemperature();

  delay(1);
}

void checkTemperature() {
  if (millis() - lastUpdate < timeBetweenUpdates) {
    // not time to update yet
    return;
  }
  lastUpdate = millis();
  int check = DHT.read11(SENSOR_PIN);
  if (check != 0 /* OK */) {
    Serial.println("error reading dht!");
    return;
  }
  int temperature = (int)DHT.temperature;
  sprintf(ssid, "Temperature_%02d", temperature);
  Serial.println(ssid);

  WiFi.softAP(ssid, password); // update wifi name
}

