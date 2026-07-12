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
  // Attempts to read the sensor up to five times while the values are still invalid.
  while ((isnan(t) || isnan(h)) && (i < 5))
  {
    // Waits briefly for the sensor to stabilise before taking another reading.
    delay(2000);
    // Reads the temperature and humidity directly from the DHT11 sensor.
    t = dht.readTemperature();
    h = dht.readHumidity();
    i = i + 1;
  }

  // If valid data is still unavailable after the attempts, it responds with a 503 error.
  if (isnan(t) || isnan(h))
  {
    server.send(503, "application/json",
                "{\"error\":\"Sensor read failed after 5 attempts\"}");
    return false;
  }

  // Builds the response body in JSON format with formatted values.
  String json = "{";
  json += "\"temperature_c\":" + String(t, 1) + ",";
  json += "\"humidity_pct\":" + String(h, 1);
  json += "}";

  // Sends the JSON response to the client that requested the data.
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
  // Initialises serial communication for diagnostic messages.
  Serial.begin(9600);
  delay(2000);

  Serial.println("\n=== WiFi Server ===");

  // Initialises the DHT11 sensor once the system is ready.
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

  // Connects the ESP8266 to the Wi-Fi network defined in the secret credentials.
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  // Waits until the Wi-Fi connection is active before continuing.
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Registers the HTTP routes that the server will respond to.
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.onNotFound(handleNotFound);

  // Starts the web server on port 80 of the ESP8266.
  server.begin();
  Serial.println("HTTP server started on port 80");
  delay(2000);
}

void loop()
{
  // Handles incoming HTTP requests while the server is active.
  server.handleClient();
}