#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 4    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    dht.begin();
}


void loop()
{
    // READ DATA
    Serial.println("loop");

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    Serial.print("T: ");
    Serial.println(t);
    Serial.print("H: ");
    Serial.println(h);
    Serial.println("");

    delay(2000);
}


// -- END OF FILE --

