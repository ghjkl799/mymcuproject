// Joins home Wi-Fi and serves a control page + JSON API for the built-in LED,
// a relay module and a DHT22 temperature/humidity sensor.
// Board: NodeMCU 1.0 (ESP-12E), FQBN esp8266:esp8266:nodemcuv2
// Libraries: "DHT sensor library" + "Adafruit Unified Sensor"
//
// Credentials come from the private library Secrets.h (NOT in this repo):
//   %USERPROFILE%\Documents\Arduino\libraries\Secrets\Secrets.h
// Template: samples/Secrets.example.h
//
// Wiring:
//   DHT22-M module:      +  -> 3V3,      OUT -> D2 (GPIO4),  - -> GND
//   RELC-1CH-5V-UNI:     DC+ -> VIN (5V), IN  -> D5 (GPIO14), DC- -> GND
//                        trigger jumper on HIGH (GPIO HIGH = relay on, NO closed)
//
// API:
//   GET  /api/led                        -> {"led":"on"|"off"}
//   POST /api/led?state=on|off|toggle    -> new state as JSON
//   GET  /api/relay                      -> {"relay":"on"|"off"}
//   POST /api/relay?state=on|off|toggle  -> new state as JSON
//   GET  /api/sensor                     -> {"ok":true,"temperature":23.4,"humidity":45.6,"age_ms":812}
//                                           {"ok":false,"error":"...","age_ms":...} if no valid reading

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Secrets.h>
#include "index_html.h"

const uint8_t DHT_PIN = 4;                      // D2
const unsigned long SENSOR_INTERVAL_MS = 2500;  // DHT22 needs >= 2 s between reads

ESP8266WebServer server(80);
DHT dht(DHT_PIN, DHT22);

// An on/off output with its own API endpoint.
struct Output {
  const char* name;   // used in URL (/api/<name>) and JSON key
  uint8_t pin;
  uint8_t onLevel;    // pin level that means "on"
  bool on;
};

Output led   = { "led",   LED_BUILTIN, LOW,  false };  // built-in LED is active LOW
Output relay = { "relay", 14,          HIGH, false };  // D5, module in HIGH-trigger mode

void setOutput(Output& o, bool on) {
  o.on = on;
  digitalWrite(o.pin, on ? o.onLevel : !o.onLevel);
}

void sendOutputState(Output& o) {
  char buf[40];
  snprintf(buf, sizeof(buf), "{\"%s\":\"%s\"}", o.name, o.on ? "on" : "off");
  server.send(200, "application/json", buf);
}

void handleOutputSet(Output& o) {
  String s = server.arg("state");
  if      (s == "on")     setOutput(o, true);
  else if (s == "off")    setOutput(o, false);
  else if (s == "toggle") setOutput(o, !o.on);
  else {
    server.send(400, "text/plain", "state must be on|off|toggle");
    return;
  }
  Serial.printf("%s -> %s (from %s)\n", o.name, o.on ? "ON" : "OFF",
                server.client().remoteIP().toString().c_str());
  sendOutputState(o);
}

void registerOutput(Output& o) {
  pinMode(o.pin, OUTPUT);
  setOutput(o, false);
  String path = String("/api/") + o.name;
  server.on(path, HTTP_GET,  [&o] { sendOutputState(o); });
  server.on(path, HTTP_POST, [&o] { handleOutputSet(o); });
}

// Latest sensor reading, refreshed in loop(); the API serves this cached value
// so HTTP requests never wait on the (slow) sensor.
float temperature = NAN;
float humidity = NAN;
unsigned long lastGoodRead = 0;   // millis() of last valid reading, 0 = never
unsigned long lastReadAttempt = 0;

void handleSensor() {
  char buf[128];
  unsigned long now = millis();
  if (lastGoodRead == 0) {
    snprintf(buf, sizeof(buf), "{\"ok\":false,\"error\":\"no reading yet - check wiring\"}");
  } else if (now - lastGoodRead > 3 * SENSOR_INTERVAL_MS) {
    snprintf(buf, sizeof(buf),
             "{\"ok\":false,\"error\":\"sensor not responding\",\"age_ms\":%lu}", now - lastGoodRead);
  } else {
    snprintf(buf, sizeof(buf),
             "{\"ok\":true,\"temperature\":%.1f,\"humidity\":%.1f,\"age_ms\":%lu}",
             temperature, humidity, now - lastGoodRead);
  }
  server.send(200, "application/json", buf);
}

void readSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("DHT22 read failed");
    return;
  }
  temperature = t;
  humidity = h;
  lastGoodRead = millis();
  if (lastGoodRead == 0) lastGoodRead = 1;  // keep 0 reserved for "never"
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void setup() {
  // Outputs first, so the relay is driven OFF as early as possible after boot.
  registerOutput(relay);
  registerOutput(led);

  Serial.begin(74880);
  delay(100);
  Serial.println();

  dht.begin();

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
  server.on("/api/sensor", HTTP_GET, handleSensor);
  server.onNotFound([] { server.send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.printf("HTTP server started: http://%s/\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastReadAttempt >= SENSOR_INTERVAL_MS) {
    lastReadAttempt = now;
    readSensor();
  }
}
