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
    // Inicia a comunicação serial para exibir mensagens de depuração no monitor.
    Serial.begin(115200);
    delay(1000);

    // Configura o pino do LED como saída e garante que ele comece desligado.
    pinMode(FLASH_LED_PIN, OUTPUT);
    digitalWrite(FLASH_LED_PIN, LOW);

    // Inicializa a comunicação SPI com os pinos usados pelo módulo RFID.
    SPI.begin(14, 12, 13, 2);
    // Inicializa o módulo MFRC522 para começar a leitura de cartões.
    mfrc522.PCD_Init();

    Serial.println("\n=== RFID Lock Test ===");
    Serial.println("System Ready. Please scan a card...");
}

void loop()
{
    // Verifica se há um novo cartão presente e se conseguiu ler seu identificador.
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print("Card Scanned! UID:");
    // Assume que o cartão é o mestre até encontrar algum byte diferente do UID autorizado.
    bool isMasterCard = true;

    // Percorre todos os bytes do UID lido para compará-los com o UID autorizado.
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        // Adiciona um zero à esquerda para manter a formatação hexadecimal legível.
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Marca o cartão como não autorizado se qualquer byte for diferente.
        if (mfrc522.uid.uidByte[i] != authorisedUID[i])
        {
            isMasterCard = false;
        }
    }
    Serial.println();

    // Se todos os bytes corresponderem, libera o acesso e acende o LED.
    if (isMasterCard)
    {
        Serial.println("Access Granted! Opening lock...");

        // Faz o LED piscar algumas vezes para indicar que o acesso foi concedido.
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
    // Para a comunicação com o cartão após a leitura para liberar o módulo.
    mfrc522.PICC_HaltA();
    delay(1000);
}