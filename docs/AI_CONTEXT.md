# AI Context — PIFKID 2026 Smart Desk Clock

**Purpose:** Complete project summary for AI coding sessions
**Last Updated:** 2026-07-28
**Lines:** <500

---

## 1. Project Overview

A WiFi-connected smart desk clock with:
- TFT display (240×320, landscape)
- Rotary encoder for navigation
- 3 physical buttons
- Piezo speaker for alarms
- Battery monitoring
- Web-based configuration UI

**MCU:** ESP32 Dev Module
**Framework:** Arduino
**Architecture:** Feature-based vertical slices

---

## 2. Current State

| Component | Status | Notes |
|-----------|--------|-------|
| Web UI | ✅ Done | SPA, vanilla JS, 87KB total |
| Firmware | ✅ Done | ~4,500 LoC, ~50 files |
| API | ✅ Done | 15 REST endpoints |
| Hardware | 🔄 Pending | Awaiting real hardware test |
| Documentation | ✅ Done | Complete |

**Phase:** Hardware bring-up & validation
**Architecture:** FROZEN

---

## 3. System Architecture

```
Web UI ←HTTP→ Handlers → Services → Repositories → LittleFS
                         ↕
                      EventBus
                         ↕
                       HAL → Hardware
```

**Key principle:** All inter-module communication goes through EventBus. No direct coupling between services.

---

## 4. Technology Stack

| Layer | Technology |
|-------|-----------|
| MCU | ESP32 (Dual-core, 240MHz, 320KB RAM, 4MB Flash) |
| Framework | Arduino |
| Display | TFT_eSPI (ST7735/ILI9341) |
| Web Server | ESPAsyncWebServer + AsyncTCP |
| JSON | ArduinoJson v6 |
| Filesystem | LittleFS |
| Threading | FreeRTOS (mutexes, queues) |
| Language | C++ (Arduino dialect) |

---

## 5. Key Files

### Entry Point
- `firmware/main.ino` — setup() and loop()

### Configuration
- `firmware/src/core/config.h` — All constants
- `firmware/src/core/pin_config.h` — Pin assignments

### Event System
- `firmware/src/events/event_bus.h` — 22 event types
- `firmware/src/events/event_bus.cpp` — Thread-safe queue

### Data Layer
- `firmware/src/repositories/*_repo.h` — CRUD with mutex
- `firmware/src/models/models.h` — Data structures

### Business Logic
- `firmware/src/services/*_service.h` — Feature logic

### API Layer
- `firmware/src/handlers/*_handlers.h` — HTTP handlers
- `firmware/src/server/server.cpp` — Route registration

### Hardware Abstraction
- `firmware/src/hal/display_hal.h` — TFT driver
- `firmware/src/hal/encoder_hal.h` — Rotary encoder
- `firmware/src/hal/speaker_hal.h` — Alarm sounds
- `firmware/src/hal/tft_manager.h` — Screen routing

### Frontend
- `data/index.html` — Single page app
- `data/js/core/store.js` — State management
- `data/js/core/router.js` — Hash routing
- `data/js/api/client.js` — API wrapper

---

## 6. Data Models

### Todo
```cpp
struct Todo {
  uint16_t id;
  char title[64];
  char description[128];
  char color[12];
  bool completed;
  unsigned long createdAt;
};
```

### Alarm
```cpp
struct Alarm {
  uint16_t id;
  uint8_t hour;
  uint8_t minute;
  bool repeatDays[7]; // Sun-Sat
  bool enabled;
  char sound[16];
  uint8_t volume;
};
```

### ScheduleEntry
```cpp
struct ScheduleEntry {
  uint16_t id;
  uint8_t day;       // 0=Sun
  char startTime[6]; // HH:MM
  char endTime[6];
  char title[64];
  char color[12];
};
```

---

## 7. EventBus Events

| Category | Events |
|----------|--------|
| Navigation | EVT_NAV_ENCODER, EVT_NAV_SELECT, EVT_NAV_BACK, EVT_SCREEN_CHANGED |
| Todo | EVT_TODO_CREATED/UPDATED/DELETED/TOGGLED |
| Alarm | EVT_ALARM_CREATED/UPDATED/DELETED/TRIGGERED/STOPPED |
| Schedule | EVT_SCHEDULE_CREATED/UPDATED/DELETED |
| System | EVT_TIME_CHANGED, EVT_WIFI_CONNECTED/DISCONNECTED, EVT_SOUND_CHANGED |

**Flow:** Service emits → EventBus queues → processQueue() dispatches → Subscribers react

---

## 8. Thread Safety Model

| Layer | Protection |
|-------|-----------|
| Repositories | `SemaphoreHandle_t _mutex` per repo |
| EventBus | Separate mutexes for queue and subscriptions |
| Services | Access repos only (repos are thread-safe) |
| Handlers | Access services only (services are safe) |

**Rule:** Never hold two mutexes simultaneously. Lock timeout: 100ms.

---

## 9. WiFi State Machine

```
IDLE → AP_MODE (beginAP)
IDLE → STA_CONNECTING (startConnectSTA)
STA_CONNECTING → STA_CONNECTED (success)
STA_CONNECTING → STA_DISCONNECTED (timeout)
STA_DISCONNECTED → RECONNECTING (auto)
RECONNECTING → STA_CONNECTING (backoff)
RECONNECTING → CONNECTION_FAILED (max retries)
```

**Config:** Exponential backoff 1s→30s, max 10 retries, ±20% jitter.

---

## 10. API Response Format

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

## 11. Build Commands

```bash
# Compile
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/

# Upload firmware
arduino-cli upload --fqbn esp32:esp32:esp32 --port COM3 firmware/

# Upload LittleFS (via Arduino IDE)
Tools > ESP32 Sketch Data Upload

# Serial monitor
115200 baud
```

---

## 12. Development Rules

### DO
- Fix only verified hardware bugs
- Produce verification reports
- Compile after each change
- Document new bugs

### DO NOT
- Add new features
- Refactor architecture
- Optimize unless verified issue
- Modify module boundaries
- Change EventBus event types
- Remove mutex protection

---

## 13. File Naming Convention

| Type | Convention | Example |
|------|-----------|---------|
| Headers | snake_case.h | `todo_repo.h` |
| Source | snake_case.cpp | `todo_repo.cpp` |
| Constants | UPPER_SNAKE | `MAX_TODOS` |
| Classes | PascalCase | `TodoRepository` |
| Instances | camelCase | `todoRepo` |
| Log tags | 8-char max | `"TODO_REPO"` |

---

## 14. Memory Budget

| Resource | Budget | Used |
|----------|--------|------|
| Flash | 4MB | ~431KB (10.5%) |
| RAM | 320KB | ~107KB (33%) |
| Heap (min free) | >80KB | TBD |
| Web UI | <150KB | 87KB |

---

## 15. Performance Targets

| Metric | Target |
|--------|--------|
| Frame rate | ≥8 FPS |
| Frame time | ≤15ms avg, ≤30ms worst |
| API latency | ≤100ms |
| Boot time | ≤5.5s to clock display |
| WiFi reconnect | ≤10s |

---

## 16. Hardware Pin Map

| Pin | Function |
|-----|----------|
| 2 | Status LED |
| 4 | TFT Backlight |
| 5 | TFT CS |
| 16 | TFT DC |
| 17 | TFT RST |
| 18 | TFT SCLK |
| 21 | Speaker (LEDC) |
| 23 | TFT MOSI |
| 25 | Encoder A |
| 26 | Encoder B |
| 27 | Encoder Button |
| 32 | Button 1 |
| 33 | Button 2 |
| 34 | Battery ADC |

---

## 17. Common Tasks

### Add a new log message
```cpp
logger.info("TAG", "Message %s", value);
```

### Emit an event
```cpp
eventBus.emit(EVT_TODO_CREATED, todoId);
```

### Subscribe to an event
```cpp
eventBus.subscribe(EVT_TODO_CREATED, [](const Event& e) {
  // Handle event
});
```

### Access repository (thread-safe)
```cpp
auto todos = todoRepo.getAll(); // Returns copy
Todo t = todoRepo.getById(id);
```

---

## 18. Troubleshooting

| Symptom | Likely Cause |
|---------|-------------|
| No serial output | Check COM port, baud rate 115200 |
| TFT white screen | Check SPI wiring, TFT_eSPI config |
| WiFi won't connect | Check SSID/password in config |
| Alarm doesn't sound | Check speaker wiring, pin 21 |
| Web UI won't load | Check LittleFS upload |

---

*End of AI Context*
