// 1. Put at the very top
#define BUZZER_PIN 4

// 2. Put inside void setup()
pinMode(BUZZER_PIN, OUTPUT);
noTone(BUZZER_PIN); // Ensure it is silent at startup

// 3. Put inside void loop() 
// If PM2.5 > 70 OR Gas > 5000 (Red Scale / Very Bad)
if (pm25_val >= 70 || tvoc_ppb >= 5000) {
  tone(BUZZER_PIN, 1000); // Play a 1000Hz tone
} else {
  noTone(BUZZER_PIN);     // Stop the sound if air is clean
}