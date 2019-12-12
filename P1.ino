#define MQTT_KEEPALIVE 10

#include <stdio.h>                
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// Wifi manager
#include <DNSServer.h>            // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "P1_settings.h"
#include "P1_CRC.h"
#include "P1_mqtt.h"
#include "P1_telegram.h"

long lastMqttPublish = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(200);

  char buffer[64];
  sprintf (buffer, "Starting %s, %s\n", SKETCH_NAME, VERSION);
  Serial.print(buffer);

  setupWifi();
  configureOTA();
  configureMqttClient();
}

void loop() {
  ArduinoOTA.handle();
  
  if (connectMqtt()) {
    mqttClient.loop();
  }

  if (readTelegram()) {
    long now = millis();
    if (now - lastMqttPublish > MQTT_PUBLISH_INTERVAL) {
      publishDataToMqtt();
      lastMqttPublish = now;    
    }
  }
}

void configureOTA() {
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(DEFAULT_HOSTNAME);

  // No authentication by default
  ArduinoOTA.setPassword(DEFAULT_AP_PASSWORD);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
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
}

void setupWifi() {
  Serial.print("Starting WiFiManager\n");

  WiFiManager wifiManager;
  
  if (!wifiManager.autoConnect(DEFAULT_AP_SSID, DEFAULT_AP_PASSWORD)) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  
  Serial.println("WiFi connection established");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
