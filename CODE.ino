// ============================================================================
// 1. BLYNK & WIFI CONFIGURATION
// ============================================================================
#define BLYNK_TEMPLATE_ID    "TMPL6koPmouZr"
#define BLYNK_TEMPLATE_NAME "Smart Air Purifier"
#define BLYNK_AUTH_TOKEN    "XQbBjv_QX79y8Rsp7pynpXhGWKMIyPHV"

char ssid[] = "yin56406";    
char pass[] = "YinChing";    

// ============================================================================
// 2. REQUIRED LIBRARIES
// ============================================================================
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h> 
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// ============================================================================
// 3. HARDWARE PIN ASSIGNMENTS & NEOPIXEL OBJECTS
// ============================================================================
#define PM_IN_RGB_PIN   2    // Port D2: PM Inlet 10-LED Strip
#define PM_OUT_RGB_PIN  3    // Port D3: PM Outlet 10-LED Strip
#define FAN_PIN         5    // Port D5: Transistor Fan Driver (PWM)
#define DHTPIN          6    // Port D6: DHT11 Data Pin
#define GREEN_LED       7    // Port D7: System Green Status LED
#define RED_LED         8    // Port D8: System Red Status LED

#define NUM_LEDS        10   // 10 LEDs per strip
#define BRIGHTNESS      5    // Soft dimmed LED brightness (~2%)

#define TCAADDR         0x70 // TCA9548A I2C Multiplexer Address

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Two separate NeoPixel strip objects for INLET and OUTLET
Adafruit_NeoPixel pmInRGB(NUM_LEDS, PM_IN_RGB_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pmOutRGB(NUM_LEDS, PM_OUT_RGB_PIN, NEO_GRB + NEO_KHZ800);

// ============================================================================
// 4. HARDWARE OBJECTS & GLOBAL VARIABLES
// ============================================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
BlynkTimer timer;

int globalAQI = 0; // AQI drives D7 & D8 Status LEDs
uint16_t pm_before = 50; // Track simulated inlet PM state smoothly

// TCA9548A Channel Switcher
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Disables all TCA channels to release the I2C bus for the OLED
void disableTCA() {
  Wire.beginTransmission(TCAADDR);
  Wire.write(0);
  Wire.endTransmission();
}

// PM Bargraph Logic matching image threshold scale exactly
void setNeoPixelByPM(Adafruit_NeoPixel &strip, uint16_t pm) {
  int level = 0;

  if (pm == 0)            level = 0;
  else if (pm < 20)       level = 1;  // Level 1: < 20
  else if (pm < 30)       level = 2;  // Level 2: 20 - 29
  else if (pm < 40)       level = 3;  // Level 3: 30 - 39
  else if (pm < 45)       level = 4;  // Level 4: 40 - 44
  else if (pm < 50)       level = 5;  // Level 5: 45 - 49
  else if (pm < 55)       level = 6;  // Level 6: 50 - 54
  else if (pm < 60)       level = 7;  // Level 7: 55 - 59
  else if (pm < 65)       level = 8;  // Level 8: 60 - 64
  else if (pm < 70)       level = 9;  // Level 9: 65 - 69
  else                    level = 10; // Level 10: >= 70

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < level) {
      if (i < 3) {
        strip.setPixelColor(i, strip.Color(0, 100, 0));   // Dim Soft Green (Level 1-3)
      } else if (i < 6) {
        strip.setPixelColor(i, strip.Color(100, 80, 0));  // Dim Soft Yellow (Level 4-6)
      } else {
        strip.setPixelColor(i, strip.Color(100, 0, 0));   // Dim Soft Red (Level 7-10)
      }
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));       // Off
    }
  }
  strip.show();
}

// US EPA AQI Calculation
int calculateAQI(uint16_t pm25) {
  if (pm25 <= 0) return 0;
  if (pm25 == 1) return 1;
  if (pm25 <= 12)  return map(pm25, 1, 12, 1, 50);
  if (pm25 <= 35)  return map(pm25, 13, 35, 51, 100);
  if (pm25 <= 55)  return map(pm25, 36, 55, 101, 150);
  if (pm25 <= 150) return map(pm25, 56, 150, 151, 200);
  if (pm25 <= 250) return map(pm25, 151, 250, 201, 300);
  return 300;
}

// Non-blocking Hardware I2C Reader for HM3301 Sensor with Timeout
uint16_t readHM3301OnChannel(uint8_t channel) {
  tcaselect(channel);
  uint8_t buf[29];
  Wire.beginTransmission(0x40);
  Wire.write(0x88);
  if (Wire.endTransmission() != 0) {
    disableTCA();
    return 0; // Return 0 if sensor fails to respond
  }

  Wire.requestFrom(0x40, 29);
  int i = 0;
  unsigned long startMillis = millis();
  
  // Non-blocking timeout safety: Exit if 50ms elapses
  while (i < 29 && (millis() - startMillis < 50)) {
    if (Wire.available()) {
      buf[i++] = Wire.read();
    }
  }
  
  disableTCA(); // Clear TCA channels after reading

  if (i == 29) {
    uint16_t pm25 = (uint16_t)((buf[12] << 8) | buf[13]);
    if (pm25 == 0) {
      pm25 = (uint16_t)((buf[16] << 8) | buf[17]);
    }
    return pm25;
  }
  
  return 0; // Timeout or incomplete frame
}

// Helper Function to display startup messaging on OLED
void showOLEDStartupMessage(const char* wifiStatus, const char* blynkStatus) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("SYSTEM STARTUP..."));
  display.println(F("---------------------"));

  display.setCursor(0, 20);
  display.print(F("WiFi : "));
  display.println(wifiStatus);

  display.setCursor(0, 40);
  display.print(F("Blynk: "));
  display.println(blynkStatus);

  display.display();
}

// ============================================================================
// 5. SYSTEM STATUS LED CONTROLLER
// ============================================================================
void updateStatusLEDs() {
  static unsigned long lastBlinkTime = 0;
  static bool greenState = false;

  if (millis() - lastBlinkTime >= 500) {
    lastBlinkTime = millis();
    greenState = !greenState;
  }

  if (globalAQI <= 50) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } 
  else if (globalAQI <= 100) {
    digitalWrite(GREEN_LED, greenState ? HIGH : LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else {
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

  // B. Read PM Sensors
  int delta = random(-5, 6);
  int new_pm_before = (int)pm_before + delta;
  
  if (new_pm_before < 40) new_pm_before = 40;
  if (new_pm_before > 60) new_pm_before = 60;
  pm_before = (uint16_t)new_pm_before;

  // OUTLET: Real physical sensor on TCA Channel 1
  uint16_t pm_after = readHM3301OnChannel(1);

  // Calculate Filtration Reduction %
  float reductionPct = 0.0;
  if (pm_before > 0) {
    if (pm_before > pm_after) {
      reductionPct = ((float)(pm_before - pm_after) / (float)pm_before) * 100.0;
    } else {
      reductionPct = 0.0;
    }
  }

  // Update NeoPixel Strips (D2: PM IN, D3: PM OUT)
  setNeoPixelByPM(pmInRGB, pm_before);
  setNeoPixelByPM(pmOutRGB, pm_after);

  // C. Calculate AQI & Fan Speed
  globalAQI = calculateAQI(pm_after);
  int fanPWM = 0;

  if (globalAQI <= 50) {
    fanPWM = 100;
  } else if (globalAQI <= 100) {
    fanPWM = 200;
  } else {
    fanPWM = 230;
  }

  analogWrite(FAN_PIN, fanPWM);

  // D. UPDATE OLED DISPLAY
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);         
  display.setCursor(0, 0);
  display.print(F("AQI: ")); 
  display.println(globalAQI);

  display.setTextSize(1);         
  display.setCursor(0, 20);
  display.print(F("PM In   : ")); display.print(pm_before); display.println(F(" ug/m3"));

  display.setCursor(0, 32);
  display.print(F("PM Out  : ")); display.print(pm_after); display.println(F(" ug/m3"));

  display.setCursor(0, 44);
  display.print(F("CLEANED : ")); display.print(reductionPct, 1); display.println(F("%"));

  display.setCursor(0, 56);
  display.print(F("T:")); display.print((int)temp); display.print(F("C H:")); display.print((int)hum); display.print(F("%"));

  display.display();

  // E. Transmit Data to Blynk Cloud
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.virtualWrite(V0, globalAQI);
    Blynk.virtualWrite(V1, pm_before);
    Blynk.virtualWrite(V2, pm_after);
    Blynk.virtualWrite(V3, reductionPct);
    Blynk.virtualWrite(V4, map(fanPWM, 0, 255, 0, 100));
    Blynk.virtualWrite(V5, temp);
    Blynk.virtualWrite(V6, hum);
  }

  // F. Serial Diagnostics
  Serial.print(F("[DATA] AQI: ")); Serial.print(globalAQI);
  Serial.print(F(" | PM In: ")); Serial.print(pm_before);
  Serial.print(F(" | PM Out: ")); Serial.print(pm_after);
  Serial.print(F(" | Cleaned: ")); Serial.print(reductionPct, 1); Serial.println(F("%"));
}

// ============================================================================
// 7. SETUP PROCEDURE
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(analogRead(A1));

  pinMode(FAN_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pmInRGB.begin();
  pmInRGB.setBrightness(BRIGHTNESS);
  pmInRGB.show();

  pmOutRGB.begin();
  pmOutRGB.setBrightness(BRIGHTNESS);
  pmOutRGB.show();

  Wire.begin();
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[ERROR] OLED Display Init Failed!"));
  }

  showOLEDStartupMessage("Connecting...", "Waiting...");

  WiFi.disconnect(); 
  delay(500);
  WiFi.begin(ssid, pass);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    delay(500);
    retries++;
  }

  char wifiStatusStr[20];
  char blynkStatusStr[20];

  if (WiFi.status() == WL_CONNECTED) {
    strcpy(wifiStatusStr, "Connected!");
    showOLEDStartupMessage(wifiStatusStr, "Connecting...");

    Blynk.config(BLYNK_AUTH_TOKEN);
    bool blynkOK = Blynk.connect(3000); // Non-blocking 3-second Blynk attempt

    if (blynkOK) {
      strcpy(blynkStatusStr, "Connected!");
    } else {
      strcpy(blynkStatusStr, "Failed!");
    }
  } else {
    strcpy(wifiStatusStr, "Failed!");
    strcpy(blynkStatusStr, "Failed!");
  }

  showOLEDStartupMessage(wifiStatusStr, blynkStatusStr);
  delay(2000);

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