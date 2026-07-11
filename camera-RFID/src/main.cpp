#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// --- Wi-Fi Setup ---
#include "secrets.h"

WebServer server(80);

// --- RFID Setup ---
#define RST_PIN 15
#define SS_PIN 2
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- Camera Setup ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Global variables to store the latest picture in memory
uint8_t* lastImage = NULL;
size_t lastImageLength = 0;

void initialiseCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // High quality image
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
}

void captureImage() {
  Serial.println("Card authorised. Taking picture...");
  camera_fb_t * fb = esp_camera_fb_get();
  
  if (!fb) {
    Serial.println("Camera capture failed!");
    return;
  }

  // Clear the old picture from memory if it exists
  if (lastImage != NULL) {
    free(lastImage);
  }

  // Allocate fresh memory and copy the new picture into it
  lastImage = (uint8_t*)ps_malloc(fb->len);
  if (lastImage != NULL) {
    memcpy(lastImage, fb->buf, fb->len);
    lastImageLength = fb->len;
    Serial.printf("Picture saved! Size: %zu bytes\n", lastImageLength);
  }

  // Return the hardware buffer
  esp_camera_fb_return(fb);
}

void handleRoot() {
  if (lastImage == NULL) {
    server.send(200, "text/plain", "No picture taken yet. Please scan an RFID card.");
    return;
  }
  
  // Serve the JPEG image directly to the browser
  server.setContentLength(lastImageLength);
  server.send(200, "image/jpeg", "");
  server.client().write(lastImage, lastImageLength);
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector
  Serial.begin(9600);
  delay(1000);
  
  // 1. Initialise SPI and RFID
  SPI.begin();
  mfrc522.PCD_Init();

  // 2. Initialise Camera
  initialiseCamera();

  // 3. Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("View your pictures at: http://");
  Serial.println(WiFi.localIP());

  // 4. Start Web Server
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

void loop() {
  server.handleClient(); // Keep the web server listening

  // Check for RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    captureImage();
    mfrc522.PICC_HaltA(); // Stop reading the same card repeatedly
    delay(1000); 
  }
}