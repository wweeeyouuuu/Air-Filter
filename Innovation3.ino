// 1. Put at the very top
#include "DHT.h"
#define DHTPIN 6
#define DHTTYPE DHT11 // Change to DHT22 if you bought the white sensor
DHT dht(DHTPIN, DHTTYPE);

// 2. Put inside void setup()
dht.begin();

// 3. Put inside void loop()
float temp = dht.readTemperature();
float hum = dht.readHumidity();

Serial.print("Temp: "); Serial.print(temp); Serial.print(" C | ");
Serial.print("Humidity: "); Serial.print(hum); Serial.println(" %");
// (You can also combine this with the OLED code to show Temp on screen!)