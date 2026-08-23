#include <Arduino.h>

// --- Pin Definitions ---
const int btnDamage = 13;
const int btnHeal   = 14;

// Red LEDs (Damage)
const int redLEDs[]   = {25, 26, 27}; 

// Green LEDs (Regeneration)
const int greenLEDs[] = {18, 19, 21}; 

void setup() {
  // Configure Buttons with internal pull-ups (active LOW)
  pinMode(btnDamage, INPUT_PULLUP);
  pinMode(btnHeal, INPUT_PULLUP);

  // Configure LED pins as outputs
  for (int i = 0; i < 3; i++) {
    pinMode(redLEDs[i], OUTPUT);
    pinMode(greenLEDs[i], OUTPUT);

    // Initial state: all lights OFF
    digitalWrite(redLEDs[i], LOW);
    digitalWrite(greenLEDs[i], LOW);
  }
}

void loop() {
  // --- 1. DAMAGE BUTTON PRESSED ---
  if (digitalRead(btnDamage) == LOW) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(redLEDs[i], HIGH);   // Turn ON all Red LEDs
      digitalWrite(greenLEDs[i], LOW);  // Turn OFF all Green LEDs
    }
    delay(300); // Simple debounce pause
  }

  // --- 2. HEALING BUTTON PRESSED ---
  if (digitalRead(btnHeal) == LOW) {
    
    // Process each pair from finger base (0) to finger tip (2)
    for (int i = 0; i < 3; i++) {
      
      // Step A: Blink the green LED 3 times
      for (int blink = 0; blink < 3; blink++) {
        digitalWrite(greenLEDs[i], HIGH);
        delay(250);
        digitalWrite(greenLEDs[i], LOW);
        delay(250);
      }

      // Step B: Turn Red OFF and Green ON for this segment
      digitalWrite(redLEDs[i], LOW);
      digitalWrite(greenLEDs[i], HIGH);

      // Step C: Wait 2.5 seconds before healing the next segment
      delay(2500);
    }
  }
}