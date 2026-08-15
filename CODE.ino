// ============================================================================
// 1. BLYNK & WIFI CONFIGURATION
// ============================================================================
#define BLYNK_TEMPLATE_ID   "TMPL6koPmouZr"
#define BLYNK_TEMPLATE_NAME "Smart Air Purifier"
#define BLYNK_AUTH_TOKEN    "XQbBjv_QX79y8Rsp7pynpXhGWKMIyPHV"

char ssid[] = "YinChing";               
char pass[] = "yin56406";           

// ============================================================================
// 2. REQUIRED LIBRARIES
// ============================================================================
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h> 
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// ============================================================================
// 3. HARDWARE PIN ASSIGNMENTS & NEOPIXEL OBJECTS
// ============================================================================
#define PM_RGB_PIN   2    // Port D2: PM Sensor 10-LED Strip
#define GAS_RGB_PIN  3    // Port D3: Gas Sensor 10-LED Strip
#define FAN_PIN      5    // Port D5: Transistor Fan Driver (PWM)
#define DHTPIN       6    // Port D6: DHT11 Data Pin
#define GREEN_LED    7    // Port D7: System Green Status LED
#define RED_LED      8    // Port D8: System Red Status LED
#define NUM_LEDS     10   // 10 LEDs per strip
#define BRIGHTNESS   50   // Dimmed LED brightness (~20%)

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_NeoPixel pmRGB(NUM_LEDS, PM_RGB_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel gasRGB(NUM_LEDS, GAS_RGB_PIN, NEO_GRB + NEO_KHZ800);

// ============================================================================
// 4. HARDWARE OBJECTS & GLOBAL VARIABLES
// ============================================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SGP30 sgp;
BlynkTimer timer;

int globalAQI = 0; // AQI drives D7 & D8 Status LEDs

// Corrected PM2.5 Level Mapping (1-19 = 1 light, 20-29 = 2, 30-39 = 3, etc.)
int getPMLevel(uint16_t pm) {
  if (pm < 20) return 1;  // 0-19 -> 1 light
  if (pm < 30) return 2;  // 20-29 -> 2 lights
  if (pm < 40) return 3;  // 30-39 -> 3 lights
  if (pm < 45) return 4;  // 40-44 -> 4 lights
  if (pm < 50) return 5;  // 45-49 -> 5 lights
  if (pm < 55) return 6;  // 50-54 -> 6 lights
  if (pm < 60) return 7;  // 55-59 -> 7 lights
  if (pm < 65) return 8;  // 60-64 -> 8 lights
  if (pm < 70) return 9;  // 65-69 -> 9 lights
  return 10;              // 70+   -> 10 lights
}

// Corrected Gas (TVOC) Level Mapping
int getGasLevel(uint16_t gas) {
  if (gas < 200)  return 1;  // 0-199 -> 1 light
  if (gas < 300)  return 2;  // 200-299 -> 2 lights
  if (gas < 400)  return 3;  // 300-399 -> 3 lights
  if (gas < 500)  return 4;  // 400-499 -> 4 lights
  if (gas < 1000) return 5;  // 500-999 -> 5 lights
  if (gas < 2000) return 6;  // 1000-1999 -> 6 lights
  if (gas < 3000) return 7;  // 2000-2999 -> 7 lights
  if (gas < 4000) return 8;  // 3000-3999 -> 8 lights
  if (gas < 5000) return 9;  // 4000-4999 -> 9 lights
  return 10;                 // 5000+ -> 10 lights
}

// Drive 10-LED Bargraph: Lights up 1..Level LEDs with color zones
void setNeoPixelByLevel(Adafruit_NeoPixel &strip, int level) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < level) {
      if (i < 3) {
        strip.setPixelColor(i, strip.Color(0, 255, 0));   // LEDs 1-3: GREEN
      } else if (i < 6) {
        strip.setPixelColor(i, strip.Color(255, 255, 0)); // LEDs 4-6: YELLOW
      } else {
        strip.setPixelColor(i, strip.Color(255, 0, 0));   // LEDs 7-10: RED
      }
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));       // Off
    }
  }
  strip.show();
}

// US EPA AQI Calculation from PM2.5
int calculateAQI(uint16_t pm25) {
  if (pm25 <= 12)       return map(pm25, 0, 12, 0, 50);
  else if (pm25 <= 35)  return map(pm25, 13, 35, 51, 100);
  else if (pm25 <= 55)  return map(pm25, 36, 55, 101, 150);
  else if (pm25 <= 150) return map(pm25, 56, 150, 151, 200);
  else                  return map(pm25, 151, 250, 201, 300);
}

// Absolute Humidity Compensation for SGP30 Gas Sensor
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  const float absHum = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature));
  return static_cast<uint32_t>(1000.0f * absHum);
}

// Hardware I2C Reader for HM3301 PM Sensor (Address 0x40)
uint16_t readPM25() {
  uint8_t buf[29];
  Wire.beginTransmission(0x40);
  Wire.write(0x88);
  if (Wire.endTransmission() != 0) return 0;

  Wire.requestFrom(0x40, 29);
  int i = 0;
  while (Wire.available() && i < 29) {
    buf[i++] = Wire.read();
  }
  
  if (i == 29) {
    uint16_t pm25 = (uint16_t)((buf[12] << 8) | buf[13]);
    if (pm25 == 0) {
      pm25 = (uint16_t)((buf[16] << 8) | buf[17]);
    }
    return pm25;
  }
  return 0;
}

// ============================================================================
// 5. SYSTEM STATUS LED CONTROLLER (Strictly tied to AQI)
// ============================================================================
void updateStatusLEDs() {
  static unsigned long lastBlinkTime = 0;
  static bool greenState = false;

  if (millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    greenState = !greenState;
  }

  if (globalAQI <= 50) {
    // AQI 0-50: Green Solid ON
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } 
  else if (globalAQI <= 100) {
    // AQI 51-100: Green Blinking
    digitalWrite(GREEN_LED, greenState ? HIGH : LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else {
    // AQI > 100: Red Solid ON
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

// ============================================================================
// 6. MAIN MONITORING CYCLE & OLED DISPLAY
// ============================================================================
void processAndSendSensorData() {
  // A. Read Temp & Humidity from DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp)) temp = 0;
  if (isnan(hum)) hum = 0;

  // B. Read SGP30 TVOC & Update D3 Gas Bargraph
  uint16_t tvoc = 0;
  if (sgp.IAQmeasure()) {
    sgp.setHumidity(getAbsoluteHumidity(temp, hum));
    tvoc = sgp.TVOC;
  }
  int gasLevel = getGasLevel(tvoc);
  setNeoPixelByLevel(gasRGB, gasLevel);

  // C. Read HM3301 PM Sensor & Update D2 PM Bargraph
  uint16_t pm25 = readPM25();
  int pmLevel = getPMLevel(pm25);
  setNeoPixelByLevel(pmRGB, pmLevel);

  // D. Calculate AQI & Fan PWM Speed based on AQI rules
  globalAQI = calculateAQI(pm25);
  int fanPWM = 0;

  if (globalAQI <= 50) {
    fanPWM = 100; // AQI 0-50: PWM 100
  } else if (globalAQI <= 100) {
    fanPWM = 200; // AQI 51-100: PWM 200
  } else {
    fanPWM = 230; // AQI > 100: PWM 230
  }

  analogWrite(FAN_PIN, fanPWM);

  // E. UPDATE OLED DISPLAY
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);          
  display.setCursor(0, 0);
  display.print(F("AQI: ")); 
  display.println(globalAQI);

  display.setTextSize(1);          
  display.setCursor(0, 18);
  display.print(F("PM2.5 : ")); display.print(pm25); display.println(F(" ug/m3"));

  display.setCursor(0, 29);
  display.print(F("VOC   : ")); display.print(tvoc); display.println(F(" ppb"));

  display.setCursor(0, 40);
  display.print(F("FAN   : ")); display.print(map(fanPWM, 0, 255, 0, 100)); display.println(F("%"));

  display.setCursor(0, 52);
  display.print(F("T: ")); display.print((int)temp); display.print(F("C   H: ")); display.print((int)hum); display.println(F("%"));

  display.display();

  // F. Transmit Data to Blynk Cloud
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.virtualWrite(V0, pm25);
    Blynk.virtualWrite(V1, globalAQI);
    Blynk.virtualWrite(V3, tvoc);
    Blynk.virtualWrite(V4, temp);
    Blynk.virtualWrite(V5, hum);
    Blynk.virtualWrite(V6, map(fanPWM, 0, 255, 0, 100));
  }

  // G. Serial Diagnostics Stream
  Serial.print(F("[DATA] AQI: ")); Serial.print(globalAQI);
  Serial.print(F(" | PM: ")); Serial.print(pm25); Serial.print(F(" (Lvl ")); Serial.print(pmLevel); Serial.print(F(" LEDs)"));
  Serial.print(F(" | Gas: ")); Serial.print(tvoc); Serial.print(F(" (Lvl ")); Serial.print(gasLevel); Serial.print(F(" LEDs)"));
  Serial.print(F(" | PWM: ")); Serial.print(fanPWM);
  Serial.print(F(" (")); Serial.print(map(fanPWM, 0, 255, 0, 100)); Serial.println(F("%)"));
}

// ============================================================================
// 7. SETUP PROCEDURE
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pmRGB.begin();
  pmRGB.setBrightness(BRIGHTNESS);
  pmRGB.show();

  gasRGB.begin();
  gasRGB.setBrightness(BRIGHTNESS);
  gasRGB.show();

  // Startup Sweep Test
  for (int i = 1; i <= 10; i++) {
    setNeoPixelByLevel(pmRGB, i);
    setNeoPixelByLevel(gasRGB, i);
    delay(100);
  }
  delay(500);

  Wire.begin();
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[ERROR] OLED Display Init Failed!"));
  }

  if (!sgp.begin()) {
    Serial.println(F("[WARN] SGP30 Gas Sensor Not Found!"));
  }

  WiFi.disconnect(); 
  delay(500);
  WiFi.begin(ssid, pass);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) { 
    delay(500);
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect(3000); 
  }

  timer.setInterval(2000L, processAndSendSensorData);
}

// ============================================================================
// 8. MAIN EXECUTION LOOP
// ============================================================================
void loop() {
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
  updateStatusLEDs(); 
}