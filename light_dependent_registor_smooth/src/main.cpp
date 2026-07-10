#include <Arduino.h>

const int LDR_PIN = A0;
const int LED_PIN = 9;

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  int sensor = analogRead(LDR_PIN);
  Serial.println(sensor);
  int brilho = map(sensor, 120, 700, 0, 255);
  brilho = constrain(brilho,0,255);
  analogWrite(LED_PIN, brilho);

  delay(200);
}