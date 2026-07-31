# Phase 5 — Power Consumption Estimates

## Operating Modes

| Mode | Components Active | Current Draw | Notes |
|------|-------------------|--------------|-------|
| **Deep Sleep** | RTC only | ~10μA | ESP32 ULP coprocessor |
| **Light Sleep** | CPU suspended, RTC + WiFi | ~800μA | WiFi maintains connection |
| **Idle (no display)** | CPU + WiFi | ~80mA | TFT backlight off |
| **Idle (display on)** | CPU + WiFi + TFT | ~120mA | TFT backlight at 50% |
| **Active (display + touch)** | CPU + WiFi + TFT + touch | ~130mA | If touch enabled |
| **WiFi AP mode** | CPU + WiFi AP + TFT | ~140mA | AP broadcasts |
| **WiFi STA connected** | CPU + WiFi STA + TFT | ~120mA | Connected to router |
| **Speaker playing** | CPU + WiFi + TFT + speaker | ~170mA | LEDC PWM active |
| **Alarm active** | CPU + WiFi + TFT + speaker | ~170mA | Full alarm cycle |
| **WiFi scanning** | CPU + WiFi scan + TFT | ~150mA | During network scan |
| **OTA update** | CPU + WiFi + flash write | ~130mA | During firmware update |

## Detailed Breakdown

### ESP32 Core
- Active CPU: ~30mA
- WiFi radio (TX): ~120mA
- WiFi radio (RX): ~80mA
- WiFi radio (idle): ~20mA
- Bluetooth (if enabled): ~50mA

### TFT Display (ST7735/ILI9341)
- Backlight off: ~5mA (logic only)
- Backlight 25%: ~20mA
- Backlight 50%: ~40mA
- Backlight 75%: ~60mA
- Backlight 100%: ~80mA
- Active drawing: +5mA burst

### Speaker (Piezo via LEDC)
- Silent (0% duty): ~0mA
- Playing (50% duty): ~50mA peak
- Average during alarm: ~30mA

### Sensors/Peripherals
- Encoder (idle): ~0mA (pullup only)
- Buttons (idle): ~0mA (pullup only)
- Battery ADC (idle): ~0.1mA
- Status LED: ~5mA (when ON)

## Battery Life Estimates

Assuming 1000mAh LiPo battery:

| Usage Pattern | Estimated Life |
|---------------|---------------|
| Always on (display + WiFi) | ~8 hours |
| Display dimmed (30%) | ~12 hours |
| Display timeout (30s off) | ~20 hours |
| Deep sleep (wake on alarm) | ~30 days |
| Light sleep (WiFi maintained) | ~5 days |

## Power Optimization Suggestions

| # | Optimization | Impact | Complexity |
|---|-------------|--------|------------|
| POW-1 | **Auto-dim after 30s** — reduce backlight when idle | -30mA average | Low |
| POW-2 | **Display timeout** — turn off TFT after inactivity | -40mA average | Low |
| POW-3 | **WiFi power save** — use `WiFi.setSleep(true)` | -20mA average | Low |
| POW-4 | **CPU frequency scaling** — reduce to 80MHz when idle | -10mA average | Medium |
| POW-5 | **Light sleep between frames** — `esp_light_sleep_start()` | -50mA average | High |
| POW-6 | **Partial display update** — SPI only sends changed regions | -10mA during render | Medium |
| POW-7 | **Speaker auto-stop** — stop alarm after 60s | -30mA after timeout | Low |
| POW-8 | **AP auto-disable** — disable AP when STA connected | -20mA average | Low |
| POW-9 | **Battery monitoring interval** — read ADC every 60s not 1s | Negligible | Low |
| POW-10 | **Deep sleep mode** — user-activated for extended standby | -120mA | Medium |

## Temperature Considerations

| Component | Max Temp | Notes |
|-----------|----------|-------|
| ESP32 | 85°C | Reduce clock speed if overheating |
| TFT backlight | 60°C | Derate if ambient >40°C |
| Piezo speaker | 85°C | No derating needed |
| Battery | 45°C | Charge/discharge limits |

---

*End of Power Consumption Estimates*
