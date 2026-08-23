#include <Arduino.h>

// --- Pin Definitions ---
const int btnDamage = 13;
const int btnHeal   = 14;
const int btnMotor  = 12;

const int redLEDs[]   = {25, 26, 27}; 
const int greenLEDs[] = {18, 19, 21}; 

// L298N Pins
const int enA = 32;
const int in1 = 33;
const int in2 = 22;

// Motor State Variables
bool motorRunning = false;
bool lastMotorBtnState = HIGH;

// ESP32 PWM Configuration
const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

void setup() {
  pinMode(btnDamage, INPUT_PULLUP);
  pinMode(btnHeal, INPUT_PULLUP);
  pinMode(btnMotor, INPUT_PULLUP);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Configure ESP32 PWM for Motor Speed
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(enA, pwmChannel);
  
  // Set motor to spin in one direction
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  for (int i = 0; i < 3; i++) {
    pinMode(redLEDs[i], OUTPUT);
    pinMode(greenLEDs[i], OUTPUT);
    digitalWrite(redLEDs[i], LOW);
    digitalWrite(greenLEDs[i], LOW);
  }
}

void loop() {
  // --- 1. MOTOR TOGGLE LOGIC ---
  // --- 1. MOTOR TOGGLE LOGIC ---
  bool currentMotorBtnState = digitalRead(btnMotor);
  if (currentMotorBtnState == LOW && lastMotorBtnState == HIGH) {
    motorRunning = !motorRunning; 
    
    if (motorRunning) {
      // KICKSTART: Send a high power pulse to break the friction
      ledcWrite(pwmChannel, 200); 
      delay(150); // Hold high power for just 150 milliseconds
      
      // RUNNING SPEED: Drop down to the slow, safe speed
      ledcWrite(pwmChannel, 70); 
    } else {
      ledcWrite(pwmChannel, 0);   
    }
    delay(200); // Debounce
  }
  lastMotorBtnState = currentMotorBtnState;
  // --- 2. DAMAGE LOGIC ---
  if (digitalRead(btnDamage) == LOW) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(redLEDs[i], HIGH);
      digitalWrite(greenLEDs[i], LOW);
    }
    delay(300); 
  }

  // --- 3. HEALING LOGIC ---
  if (digitalRead(btnHeal) == LOW) {
    for (int i = 0; i < 3; i++) {
      for (int blink = 0; blink < 3; blink++) {
        digitalWrite(greenLEDs[i], HIGH);
        delay(250);
        digitalWrite(greenLEDs[i], LOW);
        delay(250);
      }
      digitalWrite(redLEDs[i], LOW);
      digitalWrite(greenLEDs[i], HIGH);
      delay(2500);
    }
  }
}