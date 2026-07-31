# Hardware Reference — PIFKID 2026 Smart Desk Clock

**MCU:** ESP32 Dev Module
**Last Updated:** 2026-07-28

---

## 1. ESP32 Specifications

| Spec | Value |
|------|-------|
| CPU | Dual-core Xtensa LX6, 240MHz |
| RAM | 520KB SRAM |
| Flash | 4MB (external) |
| WiFi | 802.11 b/g/n 2.4GHz |
| Bluetooth | None used |
| GPIO | 34 usable pins |
| ADC | 18 channels, 12-bit |
| PWM | 16 LEDC channels |
| SPI | 4 SPI interfaces |

---

## 2. Pin Assignments

### TFT Display (SPI)
| Signal | GPIO | Notes |
|--------|------|-------|
| MOSI | 23 | SPI data |
| SCLK | 18 | SPI clock |
| CS | 5 | Chip select |
| DC | 16 | Data/Command |
| RST | 17 | Hardware reset |
| BL | 4 | Backlight (PWM) |

### Rotary Encoder
| Signal | GPIO | Notes |
|--------|------|-------|
| A | 25 | Quadrature A |
| B | 26 | Quadrature B |
| BTN | 27 | Push button |

### Buttons
| Signal | GPIO | Notes |
|--------|------|-------|
| BTN1 | 32 | User button 1 |
| BTN2 | 33 | User button 2 |

### Speaker
| Signal | GPIO | Notes |
|--------|------|-------|
| SPEAKER | 21 | LEDC PWM output |

### Status LED
| Signal | GPIO | Notes |
|--------|------|-------|
| LED | 2 | Onboard LED (active HIGH) |

### Battery Monitor
| Signal | GPIO | Notes |
|--------|------|-------|
| BATT_ADC | 34 | ADC1 channel, 0-3.3V |

---

## 3. TFT Display

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | TFT (ST7735/ILI9341 compatible) |
| Resolution | 240×320 pixels |
| Color Depth | RGB565 (16-bit, 65K colors) |
| Interface | SPI |
| Rotation | 1 (landscape) |
| Refresh Rate | 10 FPS target |

### Color Constants
```cpp
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_ORANGE  0xFD20
```

### Screen Layout (Landscape)
```
┌──────────────────────────────────────────────┐
│ Status Bar (24px)                            │
│ WiFi icon | Battery | Time                   │
├──────────────────────────────────────────────┤
│                                              │
│ Content Area (296px)                         │
│ Clock face / Todo list / Schedule / etc.     │
│                                              │
│                                              │
└──────────────────────────────────────────────┘
```

### Constants
```cpp
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define STATUS_BAR_HEIGHT 24
#define CLOCK_AREA_HEIGHT 120
#define CONTENT_Y_OFFSET (STATUS_BAR_HEIGHT)
#define CONTENT_HEIGHT (SCREEN_HEIGHT - STATUS_BAR_HEIGHT)
```

---

## 4. Rotary Encoder

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | Mechanical rotary encoder |
| Pulses per revolution | 20 |
| Button | Momentary push |
| Debounce | 50ms |
| Acceleration | Yes (speed increases with rotation) |

### Behavior
- **Rotate:** Navigate menus, adjust values
- **Press:** Select/confirm
- **Long press (2s):** Snooze alarm, back

### Acceleration Curve
```cpp
// Speed levels (ticks per 100ms)
Speed 0: 1 tick/frame  (slow)
Speed 1: 2 ticks/frame (medium)
Speed 2: 4 ticks/frame (fast)
Speed 3: 8 ticks/frame (very fast)
```

---

## 5. Buttons

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | Momentary push |
| Pull-up | Internal (INPUT_PULLUP) |
| Debounce | 50ms |
| Active | LOW |

### Edge Detection
```cpp
bool pressed();    // Rising edge (just pressed)
bool held();       // Still held after debounce
bool released();   // Falling edge (just released)
```

---

## 6. Speaker

### Specifications
| Parameter | Value |
|-----------|-------|
| Type | Piezo buzzer |
| Drive | LEDC PWM |
| Frequency | 1000-4000 Hz |
| Volume | 0-100% (duty cycle) |
| Channel | LEDC Channel 0 |

### Alarm Patterns
```cpp
Pattern 0: "Beep"     — Short beeps (500ms on, 500ms off)
Pattern 1: "Siren"    — Rising/falling tone (1000-3000Hz sweep)
Pattern 2: "Chirp"    — Quick chirps (200ms on, 300ms off)
Pattern 3: "Alarm"    — Continuous (500Hz, 1000Hz, 1500Hz)
```

### Non-Blocking Playback
```cpp
void update();           // Called every loop()
void alarm(uint8_t pattern, uint8_t volume);
void stop();
bool isActive() const;
```

---

## 7. Battery Monitor

### Specifications
| Parameter | Value |
|-----------|-------|
| ADC Channel | ADC1_CH6 (GPIO34) |
| Resolution | 12-bit (0-4095) |
| Voltage Range | 0-3.3V |
| Divider Ratio | 2:1 (100K/100K) |
| Update Interval | 60 seconds |

### Voltage Calculation
```cpp
int raw = analogRead(34);
float voltage = (raw / 4095.0) * 3.3 * 2.0; // With divider
```

### Battery Levels
| Voltage | Level |
|---------|-------|
| >4.0V | High |
| 3.7-4.0V | Medium |
| 3.5-3.7V | Low |
| <3.5V | Critical |

---

## 8. Status LED

### Behavior
| State | LED |
|-------|-----|
| Booting | Blinking (100ms) |
| WiFi AP Mode | Solid ON |
| WiFi Connected | OFF |
| Normal Operation | OFF |
| Error | Blinking (500ms) |

---

## 9. Power

### Estimated Consumption
| State | Current |
|-------|---------|
| Active (WiFi ON, TFT ON) | ~180mA |
| Active (WiFi OFF, TFT ON) | ~80mA |
| Deep Sleep | ~10μA |

### Power Budget
| Component | Typical |
|-----------|---------|
| ESP32 (active) | 100mA |
| TFT (backlight) | 40mA |
| TFT (logic) | 20mA |
| Encoder/buttons | 5mA |
| Speaker (active) | 15mA |
| **Total Active** | **~180mA** |

---

## 10. Wiring Diagram (Text)

```
ESP32 DevKit V1
├── TFT Display
│   ├── GPIO23 → MOSI
│   ├── GPIO18 → SCLK
│   ├── GPIO5  → CS
│   ├── GPIO16 → DC
│   ├── GPIO17 → RST
│   ├── GPIO4  → BL
│   ├── 3.3V   → VCC
│   └── GND    → GND
│
├── Rotary Encoder
│   ├── GPIO25 → A
│   ├── GPIO26 → B
│   ├── GPIO27 → BTN
│   ├── 3.3V   → VCC
│   └── GND    → GND
│
├── Buttons
│   ├── GPIO32 → BTN1
│   ├── GPIO33 → BTN2
│   ├── GND    → GND (both buttons)
│
├── Speaker
│   ├── GPIO21 → Speaker (+)
│   └── GND    → Speaker (-)
│
└── Battery Monitor
    └── GPIO34 → Voltage divider (100K/100K)
```

---

## 11. Calibration Notes

### TFT Backlight
- PWM range: 0-255
- 0 = OFF, 255 = MAX
- Adjust `TFT_BACKLIGHT_MAX` in config.h

### Encoder Sensitivity
- Default: 20 pulses/revolution
- Adjust in encoder_hal.cpp if needed

### Battery ADC
- Calibrate with known voltage source
- Adjust `BATT_VOLTAGE_DIVIDER` in config.h

---

## 12. Troubleshooting

| Symptom | Check |
|---------|-------|
| TFT white screen | SPI wiring, TFT_eSPI config, CS pin |
| TFT black screen | RST pin, backlight (GPIO4) |
| Encoder no response | A/B pins, VCC/GND, debounce |
| No speaker sound | GPIO21, piezo polarity, volume |
| WiFi won't connect | SSID/password, antenna |
| Battery shows 0% | ADC pin, voltage divider |
| LED stays ON | Check GPIO2 connection |

---

*End of Hardware Reference*
