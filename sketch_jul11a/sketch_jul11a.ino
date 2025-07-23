//dht
#include <Adafruit_Sensor.h>
#include <DHT.h>

bool debug = 0;

//command
bool hascommand = 0;
String command;

//echo
const String ECHO_ON = "ECHO_ON";
const String ECHO_OFF = "ECHO_OFF";
bool echo = 1;

//led on, off
const String LED_ON = "LED_ON";
const String LED_OFF = "LED_OFF";

//blink
const String BLINK_ON = "BLINK_ON";
const String BLINK_OFF = "BLINK_OFF";
const int ledPin = LED_BUILTIN;
const long interval = 1000;
bool blinkon = true;
int ledState = LOW;
unsigned long previousMillis = 0;


//dht
//includes
const String READ_TEMPERATURE = "READ_TEMPERATURE";
const String READ_HUMIDITY = "READ_HUMIDITY";
#define DHTPIN 4    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

void setup() {

Serial.begin(115200);
pinMode(ledPin, OUTPUT);
Serial.println("started");

}

void loop() {

blinkif();

if(read_command())
{
  if(echo)
  {
    Serial.print("echo: ");
    Serial.println(command);
  }

  if(command == LED_ON)
  {
    led_on();
  }
  else if(command == LED_OFF)
  {
    led_off();
  }
  else if(command == ECHO_ON)
  {
    echo_on();
  }
  else if(command == ECHO_OFF)
  {
    echo_off();
  }
  else if(command == READ_TEMPERATURE)
  {
    read_temperature();
  }
  else if(command == READ_HUMIDITY)
  {
    read_humidity();
  }
  else if(command == BLINK_ON)
  {
    blink_on();
  }
  else if(command == BLINK_OFF)
  {
    blink_off();
  }
}

delay(1);

}

void ifdebug(String str)
{
  if(debug){
    Serial.print("debug: ");
    Serial.println(str);
  }
}

bool read_command()
{
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}

void led_on()
{
  ifdebug("led on start");
  digitalWrite(ledPin, LOW);
}

void led_off()
{
  ifdebug("led off start");
  digitalWrite(ledPin, HIGH);
}

void echo_on()
{
  ifdebug("echo on start");
  echo = 1;
}

void echo_off()
{
  ifdebug("echo off start");
  echo = 0;
}

void read_temperature()
{
  ifdebug("read_temperature start");
  float t = dht.readTemperature();
  Serial.print("T: ");
  Serial.println(t);
}

void read_humidity()
{
  ifdebug("read_humidity start");
  float h = dht.readHumidity();
  Serial.print("H: ");
  Serial.println(h);
}

void blink_on()
{
  ifdebug("blink_on start");
  blinkon = true;
}

void blink_off()
{
  ifdebug("blink_off start");
  blinkon = false;
}

void blinkif()
{
  if(!blinkon) return;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}