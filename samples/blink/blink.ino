// Blink the built-in LED: 3 s on, 3 s off, and report state over Serial.
// Board: NodeMCU 1.0 (ESP-12E), FQBN esp8266:esp8266:nodemcuv2
// Note: on the NodeMCU the built-in LED (GPIO2) is active LOW.

const unsigned long INTERVAL_MS = 3000;
const uint8_t LED_ON  = LOW;
const uint8_t LED_OFF = HIGH;

bool ledOn = false;
unsigned long lastToggle = 0;
unsigned long cycle = 0;

void reportState() {
  Serial.printf("[%10lu ms] LED %s (cycle %lu)\n", millis(), ledOn ? "ON " : "OFF", cycle);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("blink: starting (3s on / 3s off)");

  pinMode(LED_BUILTIN, OUTPUT);
  ledOn = true;
  digitalWrite(LED_BUILTIN, LED_ON);
  lastToggle = millis();
  reportState();
}

void loop() {
  unsigned long now = millis();
  if (now - lastToggle >= INTERVAL_MS) {
    lastToggle += INTERVAL_MS;
    ledOn = !ledOn;
    if (ledOn) cycle++;
    digitalWrite(LED_BUILTIN, ledOn ? LED_ON : LED_OFF);
    reportState();
  }
}
