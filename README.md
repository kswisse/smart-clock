#  Smart Desk Clock

An ESP32-based smart desk clock with TFT display, rotary encoder, WiFi, alarms, todos, and schedule management. Features a full REST API and a single-page web application for configuration.

**Status:** MVP v1.0 — Hardware Bring-up Ready  
**Architecture:** Frozen

---

## Features

- **Clock Display** — Real-time clock with configurable timezone (NTP sync or manual)
- **Todo List** — Create, toggle, and manage tasks directly from the web UI
- **Alarms** — Multiple alarms with repeat days, sound selection, and snooze
- **Schedule** — Weekly schedule entries with color coding
- **Display Settings** — Brightness, auto-dim, screen timeout, animations
- **Sound Settings** — Alarm volume and sound selection
- **WiFi Management** — Scan, connect, disconnect from the web UI
- **Device Info** — Firmware version, chip info, battery level
- **REST API** — 18 endpoints for full device control
- **Web UI** — Offline-capable SPA served from LittleFS

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| MCU | ESP32 Dev Module (4MB flash, dual-core 240MHz) |
| Display | 2.4" ILI9341 TFT (240x320, SPI) |
| Input | Rotary encoder + 2 buttons |
| Speaker | Piezo buzzer (LEDC PWM) |
| Power | USB or battery (ADC monitoring) |

### Pin Assignments

| Signal | GPIO | Component |
|--------|------|-----------|
| MOSI | 23 | TFT SPI |
| SCLK | 18 | TFT SPI |
| CS | 5 | TFT |
| DC | 16 | TFT |
| RST | 17 | TFT |
| BL | 4 | TFT backlight |
| A | 25 | Rotary encoder |
| B | 26 | Rotary encoder |
| BTN | 27 | Encoder button |
| BTN1 | 32 | Button 1 |
| BTN2 | 33 | Button 2 |
| Speaker | 21 | Piezo buzzer |

Full hardware documentation: [docs/HARDWARE.md](docs/HARDWARE.md)

---

## Software Stack

| Layer | Technology |
|-------|-----------|
| MCU Firmware | Arduino (C++) on ESP32 |
| Web Server | ESPAsyncWebServer |
| Web UI | Vanilla JS SPA (no frameworks) |
| Storage | LittleFS (JSON persistence) |
| Display | TFT_eSPI |
| Build | PlatformIO or Arduino IDE |
| Testing | Desktop simulation (native C++17) |

---

## Folder Structure

```
plateforme/
├── firmware/                    # ESP32 firmware
│   ├── main.ino                 # Arduino entry point
│   ├── platformio.ini           # PlatformIO config
│   ├── sim_main.cpp             # Simulation entry point
│   ├── src/                     # Firmware source code
│   │   ├── core/                # Config, pin definitions, storage
│   │   ├── events/              # EventBus (pub/sub)
│   │   ├── hal/                 # Hardware abstraction (TFT, buttons, encoder, speaker)
│   │   ├── handlers/            # REST API handlers
│   │   ├── models/              # Data structures (Todo, Alarm, Schedule)
│   │   ├── navigation/          # Screen navigation FSM
│   │   ├── repositories/        # LittleFS persistence layer
│   │   ├── server/              # AsyncWebServer route registration
│   │   ├── services/            # Business logic
│   │   └── utils/               # Logger, JSON helpers, string utils
│   ├── sim/                     # Desktop simulation harness
│   │   ├── mock_headers/        # Mock Arduino/ESP32 libraries
│   │   ├── tests/               # 8 test suites (179 tests)
│   │   └── *.cpp/h              # Stubs and mocks
│   └── docs/                    # Firmware-specific documentation
├── data/                        # Web UI (uploaded to LittleFS)
│   ├── index.html               # SPA entry point
│   ├── css/                     # Stylesheets
│   ├── js/                      # JavaScript modules
│   │   ├── api/                 # API client modules
│   │   ├── components/          # UI components
│   │   ├── core/                # App bootstrap, router, store
│   │   ├── pages/               # Page modules
│   │   ├── state/               # Cache management
│   │   └── utils/               # DOM helpers, utilities
│   └── icons/                   # SVG icons
├── docs/                        # Project documentation
├── AGENTS.md                    # AI agent instructions (single source of truth)
├── CONTRIBUTING.md              # Contribution guidelines
└── LICENSE                      # MIT License
```

---

## Build Instructions

### Prerequisites

- Arduino IDE 1.8+ or 2.x, **or** PlatformIO
- ESP32 board package installed
- Libraries: ESPAsyncWebServer, AsyncTCP, ArduinoJson 6.x, TFT_eSPI

### PlatformIO (Recommended)

```bash
cd firmware

# Compile for ESP32
pio run -e esp32

# Upload firmware
pio run -e esp32 --target upload

# Upload LittleFS data
pio run -e esp32 --target uploadfs

# Run simulation tests
pio run -e simulation
.pio/build/simulation/program.exe
```

### Arduino IDE

1. Install ESP32 board package (add to Board Manager URLs):
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. Install libraries via Library Manager
3. Configure TFT_eSPI (see [docs/BUILD.md](docs/BUILD.md))
4. Open `firmware/main.ino`
5. Select board: ESP32 Dev Module
6. Compile and upload

### Flash Firmware

```bash
# PlatformIO
pio run -e esp32 --target upload

# Arduino CLI
arduino-cli upload --fqbn esp32:esp32:esp32 --port COM3 firmware/
```

### Upload LittleFS

The web UI files in `data/` must be uploaded to LittleFS:

```
Arduino IDE: Tools > ESP32 Sketch Data Upload
PlatformIO: pio run -e esp32 --target uploadfs
```

---

## Simulation

A desktop simulation verifies firmware logic without hardware:

```bash
cd firmware
pio run -e simulation
.pio/build/simulation/program.exe
```

The simulation covers:
- Boot sequence
- EventBus pub/sub
- Todo/Alarm/Schedule CRUD
- Time service
- WiFi state machine
- API flow validation

179 tests across 8 test suites.

---

## Web UI

The web UI is a single-page application served from the ESP32's LittleFS:

1. Upload `data/` to LittleFS
2. Connect to the ESP32's WiFi AP (`PIFKID-2026`, password: `12345678`)
3. Open `http://192.168.4.1` in a browser

### Pages

| Page | Description |
|------|-------------|
| Dashboard | Overview of clock, todos, alarms, schedule |
| Todo | Task management with color coding |
| Alarm | Multiple alarms with repeat and sound |
| Schedule | Weekly schedule view |
| Clock Settings | Time, timezone, NTP/manual mode |
| Display Settings | Brightness, auto-dim, timeout |
| Sound Settings | Alarm volume and sound selection |
| WiFi | Network scanning and connection |
| Device | Firmware info and battery status |

---

## REST API

18 endpoints for full device control. See [docs/API.md](docs/API.md) for details.

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/status` | System status (all data) |
| GET/POST | `/api/time` | Get/set time |
| GET/POST/PUT/DELETE | `/api/todo` | Todo CRUD |
| GET/POST/PUT/DELETE | `/api/alarm` | Alarm CRUD |
| GET/POST/PUT/DELETE | `/api/schedule` | Schedule CRUD |
| GET/POST | `/api/display` | Display settings |
| GET/POST | `/api/sound` | Sound settings |
| GET/POST | `/api/wifi` | WiFi management |
| GET | `/api/device` | Device info |

---

## Screenshots

<!-- Add screenshots here -->

*Coming soon after hardware validation.*

---

## Documentation

| Document | Description |
|----------|-------------|
| [AGENTS.md](AGENTS.md) | AI agent instructions (single source of truth) |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Contribution guidelines |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | System architecture |
| [docs/API.md](docs/API.md) | REST API reference |
| [docs/BUILD.md](docs/BUILD.md) | Build instructions |
| [docs/HARDWARE.md](docs/HARDWARE.md) | Hardware reference |
| [docs/MVP.md](docs/MVP.md) | MVP specification |
| [docs/ROADMAP.md](docs/ROADMAP.md) | Development roadmap |
| [docs/KNOWN_BUGS.md](docs/KNOWN_BUGS.md) | Known bugs |

---

## Team

<!-- Add team members here -->

*PIFKID 2026 Development Team*

---

## License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.
