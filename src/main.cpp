#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6
#endif

const char* API_URL = "http://iot-kknight.northcentralus.cloudapp.azure.com";
const unsigned long WIFI_TIMEOUT_MS = 15000;
const unsigned long SEND_EVERY_MS = 10000;

String mac;

bool connectWiFi() {
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println(" no Wi-Fi after 15 s.");
      return false;
    }
    delay(250);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address:  ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  return true;
}

int postJson(const String& path, const String& body) {
  HTTPClient http;
  http.begin(String(API_URL) + path);
  http.addHeader("Content-Type", "application/json");
  int status = http.POST(body);
  Serial.print("POST ");
  Serial.print(path);
  Serial.print(" -> ");
  Serial.println(status);
  if (status > 0) Serial.println(http.getString());
  http.end();
  return status;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello from the ESP32");
  if (connectWiFi()) {
    mac = WiFi.macAddress();
    
    postJson("/devices", "{\"mac\":\"" + mac + "\",\"name\":\"wokwi-board\",\"room\":\"lab\"}");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (!connectWiFi()) { delay(5000); return; }
    mac = WiFi.macAddress();
  }
  
  float temp = 28.0 + random(-20, 21) / 10.0;
  postJson("/readings", "{\"mac\":\"" + mac + "\",\"temp\":" + String(temp, 1) + "}");
  delay(SEND_EVERY_MS);
}