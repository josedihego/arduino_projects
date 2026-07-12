/**
 * @author José Dihego da Silva Oliveira josedihego.net
 * @brief Ultrasonic distance sensor and speaker alert sketch.
 */
#include <Arduino.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int LED_PIN = 8;
const int BUZZER_PIN = 9;
const int ISD_PLAY_PIN = 10;

void setup()
{
  // Start serial communication for debugging output.
  Serial.begin(9600);

  // Configure the ultrasonic sensor pins for triggering and receiving signals.
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configure the LED, buzzer, and audio trigger pins as outputs.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Set the initial state of the indicator components.
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(ISD_PLAY_PIN, OUTPUT);
  digitalWrite(ISD_PLAY_PIN, LOW);
}

void loop()
{
  // Send a short trigger pulse to start the ultrasonic measurement.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Hold the trigger high briefly to request a measurement pulse.
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the echo pulse length in microseconds.
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Convert the echo time into a distance estimate in centimetres.
  float distance = duration * 0.0343 / 2.0;

  // Handle cases where no valid echo is received.
  if (distance < 0)
  {
    Serial.println("Out of range.");
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    // Report the measured distance to the serial monitor.
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Trigger the alert sequence when an object is very close.
    if (distance <= 10)
    {
      // Blink the LED and toggle the buzzer a few times to indicate proximity.
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW); // ON
        delay(200);

        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, HIGH); // OFF
        delay(200);
      }
      // Send a short pulse to the audio playback pin.
      digitalWrite(ISD_PLAY_PIN, HIGH);
      delay(100); // 100 ms pulse
      digitalWrite(ISD_PLAY_PIN, LOW);
      delay(5000);
    }
    else
    {
      // Keep the system in its idle state when the object is further away.
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }

  // Pause briefly before taking the next measurement.
  delay(100);
}