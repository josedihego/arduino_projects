#include <Arduino.h>

// --- Windows XP Startup Melody Frequencies (Hz) ---
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
const long melodyInterval = 5000; // 5 seconds pause *between* plays

// --- Melody Sequencer Variables ---
bool isPlayingMelody = false;
int currentNote = 0;
unsigned long previousNoteMillis = 0;
int currentNoteDuration = 0;

int melody[] = { NOTE_DS4, NOTE_DS5, NOTE_AS4, NOTE_GS4, NOTE_DS5, NOTE_AS4 };
int noteDurations[] = { 150, 150, 150, 150, 300, 600 }; 
const int melodyLength = 6;

void setup() {
    for (int i = 0; i < 3; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }
    
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW); 
    
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
        digitalWrite(buzzerPin, HIGH); // Force the pin low to guarantee silence

    }

    // 2. --- Melody Trigger Logic ---
    if (!isPlayingMelody && (currentMillis - previousMelodyMillis >= melodyInterval)) {
        isPlayingMelody = true;
        currentNote = 0;
        previousNoteMillis = currentMillis; 
        digitalWrite(buzzerPin, HIGH); // Force the pin low to guarantee silence

    }

    // 3. --- Melody Sequencer Logic ---
    if (isPlayingMelody) {
        // Calculate the silent gap (10% of the note's duration)
        unsigned long gapTime = currentNoteDuration / 10;
        
        // 1. Has the note finished playing? (Time to be silent)
        if (currentNoteDuration > 0 && (currentMillis - previousNoteMillis >= currentNoteDuration)) {
            //noTone(buzzerPin); // Manually kill the sound
            digitalWrite(buzzerPin, HIGH); // Force the pin low to guarantee silence
        }

        // 2. Has the gap finished? (Time for the next note)
        if (currentNoteDuration == 0 || (currentMillis - previousNoteMillis >= (currentNoteDuration + gapTime))) {
            
            if (currentNote < melodyLength) {
                currentNoteDuration = noteDurations[currentNote];
                
                // Start the new note WITHOUT the duration parameter
                tone(buzzerPin, melody[currentNote]); 
                
                previousNoteMillis = currentMillis;
                currentNote++;
            } else {
                // The melody array has finished
                isPlayingMelody = false; 
                currentNoteDuration = 0; 
                
                noTone(buzzerPin);
                digitalWrite(buzzerPin, LOW);
                
                // Reset the 5-second timer NOW so we get a true 5-second gap
                previousMelodyMillis = currentMillis;
            }
        }
    }
}