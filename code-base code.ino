#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_SGP30.h"
#include <Seeed_HM330X.h>

// --- LED Stick Setup ---
#define PM_LED_PIN 2
#define GAS_LED_PIN 3
#define NUM_LEDS 10

Adafruit_NeoPixel pmStrip(NUM_LEDS, PM_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel gasStrip(NUM_LEDS, GAS_LED_PIN, NEO_GRB + NEO_KHZ800);

// --- Sensor Setup ---
Adafruit_SGP30 sgp;
HM330X pmSensor;
uint8_t pmData[30]; // Buffer for PM sensor data

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize LED strips
  pmStrip.begin();
  pmStrip.show(); // Initialize all pixels to 'off'
  gasStrip.begin();
  gasStrip.show();
  pmStrip.setBrightness(50); // Set brightness (0-255)
  gasStrip.setBrightness(50);

  // Initialize SGP30 Gas Sensor
  if (!sgp.begin()) {
    Serial.println("Error:Gas_Sensor_Not_Found");
    while (1); 
  }
  
  // Initialize HM3301 PM2.5 Sensor
  if (pmSensor.init()) {
    Serial.println("Error:PM_Sensor_Not_Found");
    while (1);
  }
}

void loop() {
  // --- 1. Read Gas Data (TVOC in ppb) ---
  if (!sgp.IAQmeasure()) {
    return;
  }
  int tvoc_ppb = sgp.TVOC;
  
  // --- 2. Read Particulate Data (PM2.5 in µg/m3) ---
  int pm25_val = 0;
  if (!pmSensor.read_sensor_value(pmData, 29)) {
    // Extract Standard PM2.5 value from the data array
    pm25_val = (pmData[6] << 8) | pmData[7]; 
  }

  // --- 3. Print values for the SERIAL PLOTTER ---
  // Format must be strictly: "Label1:Value1,Label2:Value2"
  Serial.print("PM2.5:"); 
  Serial.print(pm25_val); 
  Serial.print(",");
  Serial.print("Gas_ppb:"); 
  Serial.println(tvoc_ppb);

  // --- 4. Calculate LED levels based on Worksheet 05 limits ---
  int pmLedsToLight = calculatePMLeds(pm25_val);
  int gasLedsToLight = calculateGasLeds(tvoc_ppb);

  // --- 5. Update LED Sticks ---
  updateLEDStick(pmStrip, pmLedsToLight);
  updateLEDStick(gasStrip, gasLedsToLight);

  delay(2000); // Wait 2 seconds before next reading
}

// Helper function to map PM2.5 to LED count
int calculatePMLeds(int pmValue) {
  if (pmValue <= 10) return 1;
  if (pmValue <= 20) return 2;
  if (pmValue <= 30) return 3;
  if (pmValue <= 40) return 4;
  if (pmValue <= 45) return 5;
  if (pmValue <= 50) return 6;
  if (pmValue <= 55) return 7;
  if (pmValue <= 60) return 8;
  if (pmValue <= 65) return 9;
  return 10; // >= 70
}

// Helper function to map Gas (ppb) to LED count
int calculateGasLeds(int gasValue) {
  if (gasValue <= 100) return 1;
  if (gasValue <= 200) return 2;
  if (gasValue <= 300) return 3;
  if (gasValue <= 400) return 4;
  if (gasValue <= 500) return 5;
  if (gasValue <= 1000) return 6;
  if (gasValue <= 2000) return 7;
  if (gasValue <= 3000) return 8;
  if (gasValue <= 4000) return 9;
  return 10; // >= 5000
}

// Helper function to light up LEDs with correct colors
void updateLEDStick(Adafruit_NeoPixel &strip, int ledsOn) {
  strip.clear(); 
  
  for (int i = 0; i < ledsOn; i++) {
    if (i < 3) {
      strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green 
    } else if (i < 6) {
      strip.setPixelColor(i, strip.Color(255, 165, 0)); // Orange 
    } else {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Red 
    }
  }
  strip.show(); 
}
