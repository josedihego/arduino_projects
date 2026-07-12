/**
 * @author José Dihego da Silva Oliveira josedihego.net
 * @brief RFID access control sketch for reading cards and granting access.
 *
*/ 
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
    // Initialises serial communication to display debugging messages in the monitor.
    Serial.begin(115200);
    delay(1000);

    // Configures the LED pin as an output and ensures it starts off.
    pinMode(FLASH_LED_PIN, OUTPUT);
    digitalWrite(FLASH_LED_PIN, LOW);

    // Initialises SPI communication with the pins used by the RFID module.
    SPI.begin(14, 12, 13, 2);
    // Initialises the MFRC522 module so it can begin reading cards.
    mfrc522.PCD_Init();

    Serial.println("\n=== RFID Lock Test ===");
    Serial.println("System Ready. Please scan a card...");
}

void loop()
{
    // Checks whether a new card is present and whether its identifier could be read.
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print("Card Scanned! UID:");
    // Assumes the card is the master card until a different byte is found compared with the authorised UID.
    bool isMasterCard = true;

    // Iterates through all bytes of the read UID and compares them with the authorised UID.
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        // Adds a leading zero to keep the hexadecimal formatting readable.
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Marks the card as unauthorised if any byte differs.
        if (mfrc522.uid.uidByte[i] != authorisedUID[i])
        {
            isMasterCard = false;
        }
    }
    Serial.println();

    // If all bytes match, access is granted and the LED is switched on.
    if (isMasterCard)
    {
        Serial.println("Access Granted! Opening lock...");

        // Makes the LED flash a few times to indicate that access has been granted.
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
    // Stops communication with the card after reading to free the module.
    mfrc522.PICC_HaltA();
    delay(1000);
}