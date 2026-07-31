# Build Instructions — PIFKID 2026 Smart Desk Clock

**Last Updated:** 2026-07-28

---

## 1. Prerequisites

### Software
| Tool | Version | Purpose |
|------|---------|---------|
| Arduino IDE | 1.8+ or 2.x | Code editor |
| ESP32 Board Package | 2.x | Board definitions |
| Arduino CLI | Latest | CLI compilation (optional) |

### Arduino IDE Setup
1. Install Arduino IDE
2. Go to File > Preferences
3. Add to Additional Board URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to Tools > Board > Boards Manager
5. Search "esp32" and install

### Libraries
Install via Library Manager (Sketch > Include Library > Manage Libraries):
| Library | Author | Version |
|---------|--------|---------|
| ESPAsyncWebServer | me-no-dev | Latest |
| AsyncTCP | me-no-dev | Latest |
| ArduinoJson | Benoit Blanchon | 6.x |
| TFT_eSPI | Bodmer | Latest |

---

## 2. TFT_eSPI Configuration

After installing TFT_eSPI, edit the config file:

1. Find the TFT_eSPI library folder (usually in Documents/Arduino/libraries/TFT_eSPI)
2. Edit `User_Setup_Select.h` to include:
   ```cpp
   #include <User_Setups/Setup14_ILI9341_Parallel.h>
   ```
   Or create a custom setup file with:
   ```cpp
   #define ILI9341_DRIVER
   #define TFT_MOSI 23
   #define TFT_SCLK 18
   #define TFT_CS   5
   #define TFT_DC   16
   #define TFT_RST  17
   #define TFT_BL   4
   #define SPI_FREQUENCY  40000000
   #define SPI_READ_FREQUENCY  20000000
   ```

---

## 3. Arduino IDE Configuration

### Board Settings
| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Upload Speed | 921600 |
| Flash Frequency | 80MHz |
| Flash Mode | QIO |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Default 4MB with spiffs |
| Core 1 | Events (Arduino) |
| Core 0 | WiFi (Arduino) |
| Debug Level | None |
| PSRAM | Disabled |

---

## 4. Compile & Upload

### Step 1: Open Project
1. Open `firmware/main.ino` in Arduino IDE
2. Verify all source files are in `firmware/src/`

### Step 2: Select Board
1. Tools > Board > ESP32 Arduino > ESP32 Dev Module
2. Tools > Port > Select your COM port

### Step 3: Compile
1. Sketch > Verify/Compile
2. Check for errors in output window
3. Note flash/RAM usage

### Step 4: Upload Firmware
1. Connect ESP32 via USB
2. Sketch > Upload
3. Wait for "Done uploading"

### Step 5: Upload LittleFS (Web UI)
1. Close Serial Monitor
2. Tools > ESP32 Sketch Data Upload
3. Select `data/` folder
4. Wait for upload to complete

---

## 5. CLI Compilation (Alternative)

### Using Arduino CLI
```bash
# Install Arduino CLI
pip install arduino-cli

# Initialize
arduino-cli config init

# Add ESP32 core
arduino-cli core update-index
arduino-cli core install esp32:esp32

# Install libraries
arduino-cli lib install ESPAsyncWebServer
arduino-cli lib install AsyncTCP
arduino-cli lib install ArduinoJson
arduino-cli lib install TFT_eSPI

# Compile
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/

# Upload
arduino-cli upload --fqbn esp32:esp32:esp32 --port COM3 firmware/
```

---

## 6. Serial Monitor

### Settings
| Setting | Value |
|---------|-------|
| Baud Rate | 115200 |
| Line Endings | Newline |

### Boot Sequence
```
[BOOT] PIFKID Smart Desk Clock v1.0
[BOOT] Initializing LittleFS...
[BOOT] Loading configuration...
[BOOT] Initializing HAL...
[BOOT] Starting WiFi...
[BOOT] Starting web server...
[BOOT] System ready
```

---

## 7. First Boot Setup

### Initial State
1. On first boot, device creates default config files
2. If no WiFi configured, enters AP mode
3. AP SSID: `PIFKID-Setup`
4. AP Password: `12345678`

### Configuration Steps
1. Connect phone/computer to AP
2. Open browser to `192.168.4.1`
3. Configure WiFi settings
4. Device restarts and connects to WiFi

---

## 8. Troubleshooting

### Compile Errors

| Error | Solution |
|-------|----------|
| "ESPAsyncWebServer.h not found" | Install ESPAsyncWebServer library |
| "ArduinoJson.h not found" | Install ArduinoJson library |
| "TFT_eSPI.h not found" | Install TFT_eSPI library |
| "Multiple libraries found" | Remove duplicate libraries |
| "Board not found" | Install ESP32 board package |

### Upload Errors

| Error | Solution |
|-------|----------|
| "Failed to connect to ESP32" | Hold BOOT button during upload |
| "Timed out waiting for packet" | Check COM port, reduce speed |
| "A fatal error occurred" | Check flash size setting |

### Runtime Issues

| Issue | Solution |
|-------|----------|
| TFT white screen | Check SPI wiring, TFT_eSPI config |
| WiFi won't connect | Check SSID/password, serial logs |
| Web UI won't load | Verify LittleFS upload |
| No serial output | Check COM port, baud rate 115200 |
| Encoder not working | Check A/B/GND pins |

---

## 9. Development Workflow

### Making Changes
1. Edit source files in `firmware/src/`
2. Compile to verify
3. Upload to device
4. Test on hardware
5. Commit if working

### Testing Changes
1. Check serial output for errors
2. Verify web UI responds
3. Test relevant feature (todo, alarm, etc.)
4. Check memory usage in logs
5. Run for 10+ minutes to verify stability

---

## 10. File Upload Sizes

| Component | Size | Notes |
|-----------|------|-------|
| Firmware | ~431KB | Flash partition |
| Web UI | ~87KB | LittleFS |
| Config files | ~1KB | Created on boot |
| **Total** | **~519KB** | Well within 4MB |

---

*End of Build Instructions*
