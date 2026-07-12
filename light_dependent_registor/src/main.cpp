/**
 * @author José Dihego da Silva Oliveira - josedihego.net
 * @brief Light-dependent resistor example for Arduino.
 * Reads the LDR value from analog pin A0 and turns the built-in LED
 * on or off depending on ambient light level.
 */
#include <Arduino.h>

const int LDR_PIN = A0;     // Analog pin connected to the LDR voltage divider
const int LED_PIN = 13;     // On-board LED pin on most Arduino boards

const int THRESHOLD = 500;  // Light threshold for switching the LED on/off

void setup()
{
  // Configure the LED pin as an output and start serial diagnostics.
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  int ldrReading = analogRead(LDR_PIN);

  Serial.print("LDR reading: ");
  Serial.println(ldrReading);

  if (ldrReading < THRESHOLD)
  {
    // When the reading is below the threshold, it is dark.
    // Turn the LED on to indicate low light.
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    // When the reading is above the threshold, it is bright.
    // Turn the LED off to conserve power.
    digitalWrite(LED_PIN, LOW);
  }

  // Wait a short time before taking the next measurement.
  delay(200);
}