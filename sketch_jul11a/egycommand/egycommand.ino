#include "c:/Users/ghjkl/AppData/Local/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/cores/esp8266/Arduino.h"
const int MAX_COUNT_VALUES = 4;
bool  input_0_debug_on = true;
bool  input_1_led_on = true;
bool  input_2_pwm_relay_on = false;
int   input_3_pwm_i = 0;

const int LED_ON_VALUE = LOW;
const int LED_OFF_VALUE = HIGH;

const int RELAY_PIN = 4; //D2
const int PWM_PIN = 5; //D1

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);



  Serial.begin(115200);

  set_led();
}

void loop()
{
  if(read_serial_parse_and_set())
  {
    debugln("config values:");
    debug("input_0_debug_on: "); debugln(String(input_0_debug_on));
    debug("input_1_led_on: "); debugln(String(input_1_led_on));
    set_led();
    set_pwm_relay();
    set_pwm_i();
  }

  // digitalWrite(LED_BUILTIN, LED_ON_VALUE);
  // digitalWrite(RELAY_PIN, HIGH);
  // analogWrite(PWM_PIN, 20);

  // digitalWrite(LED_BUILTIN, LED_OFF_VALUE);
  // digitalWrite(RELAY_PIN, LOW);
  // analogWrite(PWM_PIN, 0);

  // delay(5000);

  // digitalWrite(LED_BUILTIN, LED_OFF_VALUE);
  // digitalWrite(RELAY_PIN, HIGH);
  // delay(5000);
  //analogWrite(16, 0);
  // for(int i = 1 ; i < 8 ; i++)
  // {
  //   toggle_led();
  //   //delay(5000);
  // }
}


bool read_serial_parse_and_set()
{
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    debug("input: ");
    debugln(input);
    return parse_input_and_set(input, ';');
  }
  return false;
}

bool parse_input_and_set(String input, char delimiter)
{
    String values[MAX_COUNT_VALUES];
    int count = splitString(input, ';', values);

    if(count != MAX_COUNT_VALUES){
      debugln("count not equals max");
      debug("count: ");
      debugln(String(count));
      
      return false;
    }

    input_0_debug_on = values[0] == "DEBUG_ON" ? true : false;
    input_1_led_on = values[1] == "LED_ON" ? true : false;
    input_2_pwm_relay_on = values[2] == "PWM_RELAY_ON" ? true : false;
    input_3_pwm_i = values[3].toInt();

    return true;
}

int splitString(String input, char delimiter, String output[]) {
  int valueCount = 0;
  int startIndex = 0;
  int delimiterIndex = input.indexOf(delimiter);

  while (delimiterIndex != -1 && valueCount < MAX_COUNT_VALUES) {
    output[valueCount++] = input.substring(startIndex, delimiterIndex);
    startIndex = delimiterIndex + 1;
    delimiterIndex = input.indexOf(delimiter, startIndex);
  }

  // last value (or whole string if no delimiter)
  if (valueCount < MAX_COUNT_VALUES) {
    output[valueCount++] = input.substring(startIndex);
  }

  return valueCount; // number of values filled in output[]
}

void set_led()
{
  debug("setting led to: ");
  debugln(String(input_1_led_on ? "on" : "off"));
  digitalWrite(LED_BUILTIN, input_1_led_on ? LED_ON_VALUE : LED_OFF_VALUE);
}

void set_pwm_relay()
{
  debug("setting pwm relay:");
  debugln(String(input_2_pwm_relay_on ? "on" : "off"));
  digitalWrite(RELAY_PIN, input_2_pwm_relay_on ? HIGH : LOW);
}

void set_pwm_i()
{
  debug("setting pwm i: ");

  if(input_2_pwm_relay_on)
  {
    debugln(String(input_3_pwm_i));
    analogWrite(PWM_PIN, input_3_pwm_i);
  }
  else{
    debugln("0");
    analogWrite(PWM_PIN, 0);
  }

}

void debug(String s)
{
  if(input_0_debug_on)
  {
    Serial.print(s);
  }
}

void debugln(String s)
{
  if(input_0_debug_on)
  {
    Serial.println(s);
  }
}

// void toggle_led()
// {
//   digitalWrite(LED_BUILTIN, led_on ? LED_OFF_VALUE : LED_ON_VALUE);
//   led_on = !led_on;
// }
