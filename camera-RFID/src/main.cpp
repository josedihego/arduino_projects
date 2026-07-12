#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 15
#define SS_PIN 2
#define FLASH_LED_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte authorisedUID[4] = {0x3C, 0xEA, 0xF5, 0x04};

void setup()
{
    Serial.begin(115200);
    delay(1000);

    pinMode(FLASH_LED_PIN, OUTPUT);
    digitalWrite(FLASH_LED_PIN, LOW);

    SPI.begin(14, 12, 13, 2);
    mfrc522.PCD_Init();

    Serial.println("\n=== RFID Lock Test ===");
    Serial.println("System Ready. Please scan a card...");
}

void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print("Card Scanned! UID:");
    bool isMasterCard = true;

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        if (mfrc522.uid.uidByte[i] != authorisedUID[i])
        {
            isMasterCard = false;
        }
    }
    Serial.println();

    if (isMasterCard)
    {
        Serial.println("Access Granted! Opening lock...");

        for (int i = 0; i < 3; i = i + 1)
        {
            digitalWrite(FLASH_LED_PIN, HIGH);
            delay(150);
            digitalWrite(FLASH_LED_PIN, LOW);
             delay(150);
        }
    }
    else
    {
        Serial.println("Access Denied.");
    }
    mfrc522.PICC_HaltA();
    delay(1000);
}