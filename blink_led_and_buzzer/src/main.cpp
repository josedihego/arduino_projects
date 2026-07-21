#include <Arduino.h>

// --- Windows XP Startup Frequencies (Hz) ---
#define NOTE_DS4 311
#define NOTE_GS4 415
#define NOTE_AS4 466
#define NOTE_DS5 622

// --- Pin Definitions ---
const int ledPins[] = {2, 3, 4}; 
const int buzzerPin = 5;         

// --- LED Timing Variables ---
unsigned long previousLedMillis = 0;
const long ledInterval = 1000; 
int currentLed = 0;

// --- Melody Timing Variables ---
unsigned long previousMelodyMillis = 0;
const long melodyInterval = 5000; 

// --- Melody Sequencer Variables ---
bool isPlayingMelody = false;
int currentNote = 0;
unsigned long previousNoteMillis = 0;
int currentNoteDuration = 0;

// Flag to track which melody is playing
bool playWindowsMelody = true; 

// --- Melody 1: Windows XP ---
int melody1[] = { NOTE_DS4, NOTE_DS5, NOTE_AS4, NOTE_GS4, NOTE_DS5, NOTE_AS4 };
int noteDurations1[] = { 150, 150, 150, 150, 300, 600 }; 
const int melodyLength1 = 6;

// --- Melody 2: Dial-up Modem (Approximation) ---
int melody2[] = { 1200, 1500, 1200, 1500, 2400, 2800, 2400, 2800, 3200 };
int noteDurations2[] = { 200, 200, 200, 200, 80, 80, 80, 80, 600 }; 
const int melodyLength2 = 9;


void setup() {
    for (int i = 0; i < 3; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }
    
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, HIGH); // Default to HIGH (silent for Active-Low)
    
    // Trigger melody at boot
    isPlayingMelody = true;
    currentNote = 0;
    previousNoteMillis = millis(); 
}

void loop() {
    unsigned long currentMillis = millis();
    
    // 1. --- LED Alternating Logic ---
    if (currentMillis - previousLedMillis >= ledInterval) {
        previousLedMillis = currentMillis;

        for (int i = 0; i < 3; i++) {
            digitalWrite(ledPins[i], LOW);
        }

        digitalWrite(ledPins[currentLed], HIGH);
        currentLed = (currentLed + 1) % 3;
        digitalWrite(buzzerPin, HIGH); // Force the pin high to guarantee silence
    }

    // 2. --- Melody Trigger Logic ---
    if (!isPlayingMelody && (currentMillis - previousMelodyMillis >= melodyInterval)) {
        isPlayingMelody = true;
        currentNote = 0;
        previousNoteMillis = currentMillis; 
        digitalWrite(buzzerPin, HIGH); // Force the pin high to guarantee silence
    }

    // 3. --- Melody Sequencer Logic ---
    if (isPlayingMelody) {
        // Determine which arrays to read from based on the flag
        int currentLen = playWindowsMelody ? melodyLength1 : melodyLength2;
        int currentFreq = playWindowsMelody ? melody1[currentNote] : melody2[currentNote];
        int currentDur = playWindowsMelody ? noteDurations1[currentNote] : noteDurations2[currentNote];

        unsigned long gapTime = currentNoteDuration / 10;
        
        if (currentNoteDuration > 0 && (currentMillis - previousNoteMillis >= currentNoteDuration)) {
            digitalWrite(buzzerPin, HIGH); // Force the pin high to guarantee silence
        }

        if (currentNoteDuration == 0 || (currentMillis - previousNoteMillis >= (currentNoteDuration + gapTime))) {
            
            if (currentNote < currentLen) {
                currentNoteDuration = currentDur;
                
                tone(buzzerPin, currentFreq); 
                
                previousNoteMillis = currentMillis;
                currentNote++;
            } else {
                isPlayingMelody = false; 
                currentNoteDuration = 0; 
                
                noTone(buzzerPin);
                digitalWrite(buzzerPin, HIGH); // Used HIGH here to match your hardware
                
                playWindowsMelody = !playWindowsMelody;
                
                previousMelodyMillis = currentMillis;
            }
        }
    }
}