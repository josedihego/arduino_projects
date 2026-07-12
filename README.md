# Arduino Project Collection
## Author: José Dihego da Silva Oliveira - josedihego.net

This repository contains five Arduino/PlatformIO projects demonstrating sensor input, actuator output, IoT communication, and access control.

---

## 1. `temperature_humidity_monitor`

**Description**: ESP8266-based local IoT temperature and humidity monitor.

**Hardware**
- ESP8266 board (WeMos D1 R1)
- DHT11 temperature/humidity sensor

**Functionality**
- Reads temperature and humidity from the DHT11 sensor
- Connects to Wi-Fi using credentials stored in secrets.h
- Hosts a local HTTP server on port 80
- Supports:
  - `GET /` → plain text status
  - `GET /data` → JSON with `temperature_c` and `humidity_pct`

**Notes**
- Includes a manual flashing override procedure for reliable firmware uploads
- Retries sensor reads up to 5 times before returning a `503` error

---

## 2. `camera-RFID`

**Description**: RFID access control prototype.

**Hardware**
- Arduino-compatible board
- MFRC522 RFID reader module
- Indicator LED on pin 4

**Functionality**
- Reads RFID cards via SPI
- Compares scanned UID against a hard-coded authorised UID
- If matched, prints “Access Granted” and flashes the LED
- If unmatched, prints “Access Denied”

---

## 3. `ultrassonic_sensor_and_speaker`

**Description**: Ultrasonic distance alarm with LED and buzzer.

**Hardware**
- HC-SR04 ultrasonic sensor
- Indicator LED
- Buzzer
- Audio playback trigger pin

**Functionality**
- Sends an ultrasonic trigger pulse and measures echo time
- Converts echo duration to distance in centimetres
- If an object is within 10 cm:
  - blinks the LED
  - toggles the buzzer
  - triggers the audio playback pin
- Otherwise keeps the system idle

**Notes**
- Uses `pulseIn()` for echo timing
- Includes a short delay before each measurement cycle

---

## 4. `light_dependent_registor`

**Description**: Basic light-dependent resistor detector.

**Hardware**
- Light-dependent resistor (LDR) voltage divider
- Built-in LED on pin 13

**Functionality**
- Reads the LDR value from analog pin `A0`
- If the reading is below a threshold, turns the LED on
- If the reading is above the threshold, turns the LED off
- Prints the raw sensor reading to serial

---

## 5. `light_dependent_registor_smooth`

**Description**: Smooth LED dimming using an LDR and PWM.

**Hardware**
- Light-dependent resistor (LDR) voltage divider
- LED on PWM pin 9

**Functionality**
- Reads ambient light from analog pin `A0`
- Maps the sensor value into a PWM brightness range
- Uses `analogWrite()` on a digital PWM pin to vary LED brightness
- Prints the raw sensor value to serial

**Notes**
- Includes comments explaining PWM (pulse-width modulation)
- Demonstrates the difference between analog input sensing and digital PWM output

---

## Usage

1. Open the desired project folder in PlatformIO or Arduino IDE
2. Connect the corresponding hardware
3. Build and upload the code
4. For the Wi-Fi project, configure network credentials in secrets.h

