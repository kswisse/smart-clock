# AGENTS.md — Single Source of Truth

**Project:** PIFKID 2026 Smart Desk Clock
**Milestone:** MVP v1.0 — Architecture Frozen
**Date:** 2026-07-28

---

## Project Status

| Component | Status |
|-----------|--------|
| Web UI (SPA) | ✅ Complete — 44 files, 87KB |
| Firmware Architecture | ✅ Complete — ~50 files, ~4,500 LoC |
| REST API | ✅ Complete — 15 endpoints |
| EventBus | ✅ Complete — 22 event types, thread-safe |
| Todo Feature | ✅ Complete — CRUD + TFT renderer |
| Alarm Feature | ✅ Complete — CRUD + scheduler + speaker |
| Schedule Feature | ✅ Complete — CRUD + TFT renderer |
| Hardware Abstraction | ✅ Complete — 8 HAL modules |
| TFT Manager | ✅ Complete — screen router + frame metrics |
| Speaker Service | ✅ Complete — EventBus-driven |
| Navigation | ✅ Complete — state machine |
| Race Conditions | ✅ Fixed — mutex on all repos + EventBus |
| WiFi Blocking | ✅ Fixed — non-blocking state machine |
| Double Alarm | ✅ Fixed — single trigger via EventBus |

**Current Phase:** Hardware Bring-Up & Validation
**Architecture:** FROZEN — Do not modify unless a verified bug is found

---

## Development Rules

1. **DO NOT** add new features
2. **DO NOT** refactor existing architecture
3. **DO NOT** optimize code unless a verified issue is found
4. **DO NOT** modify module boundaries
5. **DO** fix only bugs discovered during real hardware testing
6. **DO** produce verification reports for any fix
7. **DO** compile and verify after each change
8. **DO** document any new bugs found

---

## Architecture Overview

### System Layers
```
┌─────────────────────────────────────────────────────────┐
│                    Web UI (SPA)                         │
│              index.html + JS modules                    │
└─────────────────────────────────────────────────────────┘
                          │ HTTP/REST
┌─────────────────────────────────────────────────────────┐
│                  API Handlers Layer                      │
│         handlers/*.cpp — Parse requests, call services  │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                   Service Layer                         │
│     todo_service, alarm_service, schedule_service,      │
│     time_service, wifi_service, status_service,         │
│     speaker_service                                     │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                 Repository Layer                        │
│    todo_repo, alarm_repo, schedule_repo, config_repo    │
│         (All protected by FreeRTOS mutexes)             │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                    EventBus                              │
│   Thread-safe pub/sub — decouples all modules           │
│   22 event types, 64-event queue                        │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                    HAL Layer                             │
│  display_hal, encoder_hal, buttons_hal, speaker_hal,    │
│  tft_manager, tft_status_bar, tft_clock, tft_todo,     │
│  tft_schedule                                           │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                  Hardware (ESP32)                        │
│  TFT display, rotary encoder, buttons, piezo speaker    │
└─────────────────────────────────────────────────────────┘
```

### Folder Structure
```
firmware/
├── main.ino                          — Entry point (setup/loop)
├── src/
│   ├── core/
│   │   ├── config.h                  — All constants, limits, pin defs
│   │   ├── pin_config.h              — Centralized pin mapping
│   │   ├── firmware_info.h/cpp       — Version metadata
│   │   └── storage.h/cpp             — LittleFS wrapper
│   ├── events/
│   │   └── event_bus.h/cpp           — Thread-safe pub/sub
│   ├── hal/
│   │   ├── hal.h/cpp                 — Legacy HAL (LED, battery)
│   │   ├── display_hal.h/cpp         — TFT_eSPI driver
│   │   ├── buttons_hal.h/cpp         — 3-button debounce
│   │   ├── encoder_hal.h/cpp         — Rotary encoder + accel
│   │   ├── speaker_hal.h/cpp         — LEDC PWM alarm patterns
│   │   ├── tft_manager.h/cpp         — Screen router + frame metrics
│   │   ├── tft_status_bar.h/cpp      — WiFi/battery/time bar
│   │   ├── tft_clock.h/cpp           — Clock face renderer
│   │   ├── tft_todo.h/cpp            — Todo list renderer
│   │   └── tft_schedule.h/cpp        — Schedule renderer
│   ├── navigation/
│   │   └── nav_state.h/cpp           — Screen navigation FSM
│   ├── models/
│   │   └── models.h                  — Todo, Alarm, ScheduleEntry, etc.
│   ├── repositories/
│   │   ├── repository.h/cpp          — LittleFS CRUD base
│   │   ├── todo_repo.h/cpp           — Todo persistence (mutex-protected)
│   │   ├── alarm_repo.h/cpp          — Alarm persistence (mutex-protected)
│   │   ├── schedule_repo.h/cpp       — Schedule persistence (mutex-protected)
│   │   └── config_repo.h/cpp         — Device config (mutex-protected)
│   ├── services/
│   │   ├── todo_service.h/cpp        — Todo business logic
│   │   ├── alarm_service.h/cpp       — Alarm scheduler + trigger
│   │   ├── schedule_service.h/cpp    — Schedule business logic
│   │   ├── speaker_service.h/cpp     — EventBus-driven alarm sounds
│   │   ├── time_service.h/cpp        — NTP + manual time
│   │   ├── wifi_service.h/cpp        — Non-blocking WiFi state machine
│   │   └── status_service.h/cpp      — System status
│   ├── handlers/
│   │   ├── handlers.h/cpp            — Status/time handlers
│   │   ├── todo_handlers.h/cpp       — Todo REST API
│   │   ├── alarm_handlers.h/cpp      — Alarm REST API
│   │   ├── schedule_handlers.h/cpp   — Schedule REST API
│   │   └── api_response.h/cpp        — Standardized JSON responses
│   ├── server/
│   │   └── server.h/cpp              — AsyncWebServer routes
│   └── utils/
│       ├── logger.h/cpp              — Serial logging
│       ├── json_util.h/cpp           — JSON helpers
│       └── string_util.h/cpp         — String helpers
└── docs/                             — Project documentation

data/                                 — Web UI (uploaded to LittleFS)
├── index.html
├── css/
├── js/
│   ├── core/
│   ├── api/
│   ├── components/
│   ├── pages/
│   └── utils/
└── icons/
```

---

## Module Responsibilities

| Module | Responsibility |
|--------|---------------|
| `main.ino` | Boot sequence, main loop |
| `config.h` | All constants and limits |
| `pin_config.h` | All pin assignments |
| `event_bus.h/cpp` | Thread-safe pub/sub messaging |
| `repository.h/cpp` | LittleFS file operations |
| `*_repo.h/cpp` | Data persistence with mutex protection |
| `*_service.h/cpp` | Business logic, EventBus integration |
| `*_handlers.h/cpp` | REST API request/response |
| `server.h/cpp` | HTTP route registration |
| `display_hal.h/cpp` | TFT_eSPI driver wrapper |
| `encoder_hal.h/cpp` | Rotary encoder + acceleration |
| `buttons_hal.h/cpp` | 3-button debounce + edge detection |
| `speaker_hal.h/cpp` | LEDC PWM alarm patterns |
| `tft_manager.h/cpp` | Screen routing + frame timing |
| `tft_*.h/cpp` | Individual screen renderers |
| `nav_state.h/cpp` | Navigation state machine |
| `logger.h/cpp` | Serial logging with levels |

---

## Coding Rules

### File Naming
- Headers: `snake_case.h`
- Source: `snake_case.cpp`
- Constants: `UPPER_SNAKE_CASE`

### Class Naming
- PascalCase: `TodoRepository`, `AlarmService`
- Global instances: `todoRepo`, `alarmService`, `eventBus`

### Include Order
1. Own header
2. Project headers
3. Library headers

### Memory
- Use `char[]` arrays, not `Arduino String` in models
- Use `DynamicJsonDocument` with defined sizes
- Return `std::vector` by value (copy), not reference

### Thread Safety
- All repositories protected by `SemaphoreHandle_t _mutex`
- EventBus has separate mutexes for queue and subscriptions
- Never call repository methods from ISR context
- Never hold two mutexes simultaneously

### EventBus
- Emit events, don't call services directly
- Subscribe in `begin()` or `init()`
- Use string messages for sub-commands (e.g., "snooze", "stop")

---

## Architecture Constraints

1. **No business logic in HAL** — HAL = hardware driver only
2. **Services never touch filesystem** — Repository layer only
3. **Handlers never access repositories** — Service layer only
4. **All inter-module communication via EventBus** — No direct coupling
5. **FreeRTOS-safe** — No `delay()` in loop context
6. **Dirty-flag rendering** — Never full-screen redraw unless forced
7. **Offline-capable frontend** — No external CDNs
8. **Single HTML entry point** — SPA with hash routing

---

## EventBus Flow

### Event Types
```cpp
// Navigation
EVT_NAV_ENCODER, EVT_NAV_SELECT, EVT_NAV_BACK, EVT_SCREEN_CHANGED

// Todo
EVT_TODO_CREATED, EVT_TODO_UPDATED, EVT_TODO_DELETED, EVT_TODO_TOGGLED

// Alarm
EVT_ALARM_CREATED, EVT_ALARM_UPDATED, EVT_ALARM_DELETED,
EVT_ALARM_TRIGGERED, EVT_ALARM_STOPPED

// Schedule
EVT_SCHEDULE_CREATED, EVT_SCHEDULE_UPDATED, EVT_SCHEDULE_DELETED

// System
EVT_TIME_CHANGED, EVT_WIFI_CONNECTED, EVT_WIFI_DISCONNECTED,
EVT_DISPLAY_CHANGED, EVT_SOUND_CHANGED, EVT_SYSTEM_ERROR
```

### Typical Flow: API → Todo Creation
```
1. HTTP POST /api/todo → TodoHandlers::handleCreate()
2. TodoHandlers calls todoService.create()
3. TodoService calls todoRepo.create()
4. TodoRepo locks mutex, writes to LittleFS, unlocks
5. TodoService emits EVT_TODO_CREATED via eventBus
6. EventBus queues event
7. processQueue() dispatches to subscribers
8. TFT Manager receives event, invalidates todo renderer
9. Next frame: tftTodo renders updated list
```

### Typical Flow: Alarm Trigger
```
1. AlarmService::checkAlarms() detects time match
2. Emits EVT_ALARM_TRIGGERED with alarm ID
3. EventBus queues event
4. SpeakerService receives event via subscription
5. SpeakerService calls speakerHAL.alarm()
6. Speaker HAL plays pattern via LEDC PWM
```

---

## REST API Summary

### Endpoints
| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/status` | System status |
| GET | `/api/time` | Current time |
| POST | `/api/time` | Set time |
| GET | `/api/todo` | List todos |
| POST | `/api/todo` | Create todo |
| GET | `/api/todo/:id` | Get todo |
| PUT | `/api/todo/:id` | Update todo |
| DELETE | `/api/todo/:id` | Delete todo |
| GET | `/api/alarm` | List alarms |
| POST | `/api/alarm` | Create alarm |
| GET | `/api/alarm/:id` | Get alarm |
| PUT | `/api/alarm/:id` | Update alarm |
| DELETE | `/api/alarm/:id` | Delete alarm |
| GET | `/api/schedule` | List schedule |
| POST | `/api/schedule` | Create entry |
| GET | `/api/schedule/:id` | Get entry |
| PUT | `/api/schedule/:id` | Update entry |
| DELETE | `/api/schedule/:id` | Delete entry |

### Response Format
```json
{
  "success": true,
  "code": 200,
  "message": "Description",
  "data": {},
  "timestamp": 1234567890
}
```

---

## Hardware Summary

### Pin Assignments
| Component | Pins |
|-----------|------|
| TFT Display (SPI) | MOSI=23, SCLK=18, CS=5, DC=16, RST=17, BL=4 |
| Rotary Encoder | A=25, B=26, BTN=27 |
| Buttons | BTN1=32, BTN2=33 |
| Speaker | 21 (LEDC PWM) |
| Status LED | 2 |
| Battery ADC | 34 |

### Display
- Resolution: 240×320 (landscape, rotation=1)
- Color: RGB565 (16-bit)
- Refresh: 10 FPS (100ms interval)

---

## Build Instructions

### Prerequisites
- Arduino IDE 1.8+ or Arduino CLI
- ESP32 board package
- Libraries: ESPAsyncWebServer, AsyncTCP, ArduinoJson, TFT_eSPI

### Compile
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/
```

### Upload Firmware
```bash
arduino-cli upload --fqbn esp32:esp32:esp32 --port COM3 firmware/
```

### Upload LittleFS
```
Tools > ESP32 Sketch Data Upload (select data/ folder)
```

### Serial Monitor
```
115200 baud
```

---

## Known Limitations

1. No RTC hardware — time resets on power loss (NTP recovers)
2. No OTA updates — must use serial for firmware updates
3. No authentication on API — local network only
4. No persistent alarm snooze state — resets on reboot
5. Settings page is a stub — not yet implemented
6. Alarm list renderer is a stub — not yet implemented

---

## Known Bugs

None reported yet. Awaiting hardware validation.

---

## Future Roadmap

### Post-MVP (Do Not Implement Yet)
1. Settings page renderer
2. Alarm list renderer
3. OTA firmware updates
4. API authentication
5. Brightness auto-dimming
6. Deep sleep mode
7. Power optimization
8. Unit tests

---

## File Locations

| Item | Path |
|------|------|
| Firmware source | `D:\plateforme\firmware\src\` |
| Firmware entry | `D:\plateforme\firmware\main.ino` |
| Web UI | `D:\plateforme\data\` |
| Documentation | `D:\plateforme\docs\` |
| Firmware docs | `D:\plateforme\firmware\docs\` |

---

*This document is the single source of truth. All AI coding sessions must read this file first.*
