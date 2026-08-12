// 1. Letak di bahagian paling atas
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// 2. Letak di dalam void setup()
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Failed!")); 
    while(1); // Mesti berhenti di sini jika skrin gagal dipasang
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Gunakan SSD1306_WHITE untuk elak skrin gelap

// 3. Letak di dalam void loop() (Selepas bahagian updateLEDStick)
  // --- Update OLED Display ---
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.println(F("AIR QUALITY:"));
  display.setTextSize(2);
  display.print(F("PM: ")); display.println(pm25_val); 
  display.print(F("VOC: ")); display.println(tvoc_ppb); 
  display.display();
