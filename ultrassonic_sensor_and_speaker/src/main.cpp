#include <Arduino.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;
const int LED_PIN = 8;
const int BUZZER_PIN = 9;
const int ISD_PLAY = 10;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(ISD_PLAY, OUTPUT);
  digitalWrite(ISD_PLAY, LOW);
}

void loop()
{

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  float distancia = duracao * 0.0343 / 2.0;

  if (distancia < 0)
  {
    Serial.println("Fora do intervalo.");
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");

    if (distancia <= 10)
    {
      for (int i = 0; i < 3; i++)
      {

        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW); // ON
        delay(200);

        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, HIGH); // OFF
        delay(200);
      }
      digitalWrite(ISD_PLAY, HIGH);
      delay(100); // 100 ms pulse
      digitalWrite(ISD_PLAY, LOW);
      delay(5000);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }

  delay(100);
}