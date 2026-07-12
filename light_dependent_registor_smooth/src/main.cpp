/**
 * @author José Dihego da Silva Oliveira - josedihego.net
 * @brief Light-dependent resistor sketch with smooth LED dimming.
 * Reads the LDR value from analog pin A0 and adjusts the LED brightness
 * on pin 9 using PWM.
 */
#include <Arduino.h>

const int LDR_PIN = A0;  // Analog pin connected to the photoresistor divider.
                            // analogRead reads a voltage level from analog-capable pins.
const int LED_PIN = 9;   // PWM-capable digital pin used to vary the LED brightness.
                            // PWM stands for pulse-width modulation: the pin switches quickly
                            // between HIGH and LOW, and the proportion of ON time controls the
                            // perceived brightness of the LED.

void setup()
{
  // Initialise serial output for diagnostics.
  Serial.begin(9600);

  // Set the LED pin as an output.
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  int sensorValue = analogRead(LDR_PIN);
  Serial.println(sensorValue);

  // Map the sensor reading into a PWM brightness range.
  int brightness = map(sensorValue, 120, 700, 0, 255);

  // Ensure the brightness stays within the valid PWM bounds.
  brightness = constrain(brightness, 0, 255);

  // Apply the computed brightness to the LED.
  // analogWrite uses PWM (pulse-width modulation) on the digital pin to produce an
  // average voltage that the LED perceives as an analogue brightness level.
  analogWrite(LED_PIN, brightness);

  delay(200);
}