// Wi-Fi access point + web server serving "Hello World" on "/".
// Board: NodeMCU 1.0 (ESP-12E), FQBN esp8266:esp8266:nodemcuv2
// Connect to Wi-Fi "myesp" (password 123456789aA), then open http://192.168.4.1/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* AP_SSID = "myesp";
const char* AP_PASS = "123456789aA";

ESP8266WebServer server(80);

void handleRoot() {
  Serial.printf("GET / from %s\n", server.client().remoteIP().toString().c_str());
  server.send(200, "text/html", "<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  WiFi.mode(WIFI_AP);
  if (WiFi.softAP(AP_SSID, AP_PASS)) {
    Serial.printf("AP \"%s\" started, IP: %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());
  } else {
    Serial.println("AP start FAILED");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started on port 80");
}

void loop() {
  server.handleClient();
}
