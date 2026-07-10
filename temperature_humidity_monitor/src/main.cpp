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

void handleData()
{
  float t = NAN, h = NAN;

  for (int i = 0; i < 5 && (isnan(t) || isnan(h)); i++)
  {
    delay(2000);
    t = dht.readTemperature();
    h = dht.readHumidity();
  }

  if (isnan(t) || isnan(h))
  {
    server.send(503, "application/json",
                "{\"error\":\"Sensor read failed after 5 attempts\"}");
    return;
  }

  String json = "{";
  json += "\"temperature_c\":" + String(t, 1) + ",";
  json += "\"humidity_pct\":" + String(h, 1);
  json += "}";

  server.send(200, "application/json", json);
}
void handleRoot()
{
  server.send(200, "text/plain",
              "DHT11 server running. Use GET /data for readings.");
}

void handleNotFound()
{
  server.send(404, "application/json",
              "{\"error\":\"Not found\"}");
}

void setup()
{
  Serial.begin(9600);
  delay(2000);

  Serial.println("\n=== DHT11 WiFi Server ===");

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
    Serial.print("PRE-WIFI OK → t=");
    Serial.print(t);
    Serial.print(" h=");
    Serial.println(h);
  }

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // <── paste this IP into Postman

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started on port 80");
  delay(2000);
}

void loop()
{
  server.handleClient(); // process incoming HTTP requests
}