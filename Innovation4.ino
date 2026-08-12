// 1. Put at the very top
#define LIGHT_SENSOR_PIN A0

// 2. Put inside void setup()
pinMode(LIGHT_SENSOR_PIN, INPUT);

// 3. Put inside void loop() (Place this BEFORE updating the LEDs)
int lightLevel = analogRead(LIGHT_SENSOR_PIN); // Read room brightness

// If the room is very dark (sensor value drops below 300)
if (lightLevel < 300) {
  pmStrip.setBrightness(10);  // Dim the LEDs (save battery)
  gasStrip.setBrightness(10);
} else {
  pmStrip.setBrightness(50);  // Normal brightness for daytime
  gasStrip.setBrightness(50);
}
// Note: Brightness change will apply on the next loop cycle