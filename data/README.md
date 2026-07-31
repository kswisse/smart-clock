# Web UI

Single-page application served from the ESP32's LittleFS storage.

## Structure

| Directory | Purpose |
|-----------|---------|
| `js/api/` | API client modules (one per endpoint group) |
| `js/components/` | Reusable UI components |
| `js/core/` | App bootstrap, router, state store |
| `js/pages/` | Page modules (one per screen) |
| `js/state/` | Cache management |
| `js/utils/` | DOM helpers, utilities |
| `css/` | Stylesheets |
| `icons/` | SVG icons |

## Upload to ESP32

Upload the entire `data/` folder to LittleFS:

```bash
# PlatformIO
pio run -e esp32 --target uploadfs

# Arduino IDE
Tools > ESP32 Sketch Data Upload
```

## Access

1. Connect to WiFi AP: `PIFKID-2026` (password: `12345678`)
2. Open `http://192.168.4.1` in a browser

## Pages

Dashboard, Todo, Alarm, Schedule, Clock Settings, Display Settings, Sound Settings, WiFi, Device Info.
