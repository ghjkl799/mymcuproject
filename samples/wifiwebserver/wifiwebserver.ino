// Joins home Wi-Fi and serves a control page + JSON API for the built-in LED.
// Board: NodeMCU 1.0 (ESP-12E), FQBN esp8266:esp8266:nodemcuv2
//
// Credentials come from the private library Secrets.h (NOT in this repo):
//   %USERPROFILE%\Documents\Arduino\libraries\Secrets\Secrets.h
// Template: samples/Secrets.example.h
//
// API:
//   GET  /api/led                      -> {"led":"on"|"off"}
//   POST /api/led?state=on|off|toggle  -> new state as JSON

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Secrets.h>
#include "index_html.h"

ESP8266WebServer server(80);
bool ledOn = false;

void setLed(bool on) {
  ledOn = on;
  digitalWrite(LED_BUILTIN, on ? LOW : HIGH);  // NodeMCU LED is active LOW
}

void sendLedState() {
  server.send(200, "application/json", ledOn ? "{\"led\":\"on\"}" : "{\"led\":\"off\"}");
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleLedSet() {
  String s = server.arg("state");
  if      (s == "on")     setLed(true);
  else if (s == "off")    setLed(false);
  else if (s == "toggle") setLed(!ledOn);
  else {
    server.send(400, "text/plain", "state must be on|off|toggle");
    return;
  }
  Serial.printf("LED -> %s (from %s)\n", ledOn ? "ON" : "OFF",
                server.client().remoteIP().toString().c_str());
  sendLedState();
}

void setup() {
  Serial.begin(74880);
  delay(100);
  Serial.println();

  pinMode(LED_BUILTIN, OUTPUT);
  setLed(false);

  // Note: never print the SSID/password to serial.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.printf("\nConnected, IP: %s\n", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/led", HTTP_GET, sendLedState);
  server.on("/api/led", HTTP_POST, handleLedSet);
  server.onNotFound([] { server.send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.printf("HTTP server started: http://%s/\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();
}
