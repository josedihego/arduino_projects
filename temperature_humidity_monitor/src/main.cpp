/**
 * @author José Dihego da Silva Oliveira josedihego.net
 * @brief Local IoT Temperature Server for WeMos D1 R1 (ESP8266) & DHT11.
 * * =========================================================================
 * HARDWARE FLASHING OVERRIDE PROCEDURE
 * =========================================================================
 * Because the active Wi-Fi stack and local server tasks can lock the serial
 * port, follow this manual sequence to flash new firmware to the board:
 * * 1. DISCONNECT POWER AND SENSORS:
 * Unplug the USB cable and completely remove the VCC/DATA wires of the
 * DHT11 sensor from the board rails to prevent boot-up current drops.
 * * 2. FORCE FLASH MODE:
 * Connect a physical jumper wire from pin D8 (internal GPIO0) directly
 * to a GND pin.
 * * 3. REBOOT AND CONNECT:
 * Plug the USB cable back into the computer. The board will power up
 * in bootloader mode.
 * * 4. UPLOAD FIRMWARE:
 * Trigger the upload inside the IDE (PlatformIO or Arduino IDE).
 * * 5. DISCONNECT THE BYPASS WIRE:
 * The moment the terminal display changes to "Connecting........_____",
 * immediately remove the jumper wire from pin D8.
 * * 6. RECONNECT HARDWARE:
 * Once the upload finishes successfully (100%), power cycle the board
 * and plug the DHT11 sensor rails back into their working pin (GPIO2/D4).
 * =========================================================================
 * Services 
 * ngrok http 192.168.5.109:80
 * http://192.168.5.109
 * http://192.168.5.109/data
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "secrets.h"

#define DHTPIN D0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);

bool handleData()
{
  float t = NAN, h = NAN;

  int i = 0;
  // Tenta ler o sensor até 5 vezes enquanto os valores ainda forem inválidos.
  while ((isnan(t) || isnan(h)) && (i < 5))
  {
    // Espera um momento para o sensor estabilizar antes de nova leitura.
    delay(2000);
    // Lê temperatura e umidade diretamente do sensor DHT11.
    t = dht.readTemperature();
    h = dht.readHumidity();
    i = i + 1;
  }

  // Se após as tentativas ainda não houver dados válidos, responde com erro 503.
  if (isnan(t) || isnan(h))
  {
    server.send(503, "application/json",
                "{\"error\":\"Sensor read failed after 5 attempts\"}");
    return false;
  }

  // Monta o corpo da resposta em formato JSON com valores formatados.
  String json = "{";
  json += "\"temperature_c\":" + String(t, 1) + ",";
  json += "\"humidity_pct\":" + String(h, 1);
  json += "}";

  // Envia a resposta JSON para o cliente que solicitou os dados.
  server.send(200, "application/json", json);
  return true;
}
void handleRoot()
{
  server.send(200, "text/plain",
              "Server running. Use GET /data for readings.");
}

void handleNotFound()
{
  server.send(404, "application/json",
              "{\"error\":\"Not found\"}");
}

void setup()
{
  // Inicia a comunicação serial para exibir mensagens de diagnóstico.
  Serial.begin(9600);
  delay(2000);

  Serial.println("\n=== WiFi Server ===");

  // Inicializa o sensor DHT11 após o sistema estar pronto.
  dht.begin();
  delay(2000);

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h))
  {
    Serial.println("!!! FAILED before WiFi — wiring or power issue");
  }
  else
  {
    Serial.print("WIFI OK t=");
    Serial.print(t);
    Serial.print(" h=");
    Serial.println(h);
  }

  // Conecta o ESP8266 à rede Wi-Fi definida nas credenciais secretas.
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  // Aguarda até que a conexão Wi-Fi fique ativa antes de prosseguir.
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");create comments in portuguese BR for each complex (not obvious) line of code in this file.
  Serial.println(WiFi.localIP());

  // Registra as rotas HTTP que o servidor vai responder.
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.onNotFound(handleNotFound);

  // Inicia o servidor web na porta 80 do ESP8266.
  server.begin();
  Serial.println("HTTP server started on port 80");
  delay(2000);
}

void loop()
{
  // Processa requisições HTTP entrantes enquanto o servidor estiver ativo.
  server.handleClient();
}