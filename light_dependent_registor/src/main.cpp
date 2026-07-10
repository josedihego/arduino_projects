#include <Arduino.h>

const int LDR_PIN = A0;
const int LED_PIN = 13;

const int LIMITE = 500;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  int leitura_ldr = analogRead(LDR_PIN);

  Serial.print("leitura LDR: ");
  Serial.println(leitura_ldr);

  if (leitura_ldr < LIMITE)
  {
    // ambiente sem luz
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    // ambiente com luz
    digitalWrite(LED_PIN, LOW);
  }

  delay(200);
}