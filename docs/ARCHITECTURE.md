# Architecture — PIFKID 2026 Smart Desk Clock

**Version:** 1.0 (MVP Frozen)
**Date:** 2026-07-28

---

## 1. System Overview

Feature-based vertical slice architecture with clear layer separation.

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

---

## 2. Layer Rules

| Rule | Description |
|------|-------------|
| HAL → Hardware | HAL drives hardware only. No business logic. |
| Services → Repos | Services call repositories for data. Never touch filesystem directly. |
| Handlers → Services | Handlers parse requests, call services. Never access repositories. |
| EventBus → All | All inter-module communication via EventBus events. No direct coupling. |
| FreeRTOS-safe | No `delay()` in loop context. Use non-blocking patterns. |

---

## 3. Feature Vertical Slices

Each feature follows this pattern:

```
Feature
├── Repository (persistence + mutex)
├── Service (business logic + EventBus)
├── Handlers (REST API)
├── HAL/TFT Renderer (display)
└── Frontend Module (UI)
```

### Example: Todo Feature
```
todo
├── todo_repo.h/cpp     — LittleFS CRUD
├── todo_service.h/cpp  — Business logic
├── todo_handlers.h/cpp — REST API
├── tft_todo.h/cpp      — Display renderer
├── data/js/api/todo.js — API client
└── data/js/pages/todo-page.js — UI
```

---

## 4. EventBus Architecture

### Event Flow
```
Emitter → emit(event, data) → Queue → processQueue() → Subscribers
```

### Thread Safety
- Queue protected by `_queueMutex`
- Subscriptions protected by `_subMutex`
- 100ms lock timeout
- Drop events when queue full (max 64)

### Event Types
```cpp
// Navigation
EVT_NAV_ENCODER        // Encoder rotation
EVT_NAV_SELECT         // Encoder press
EVT_NAV_BACK           // Back button
EVT_SCREEN_CHANGED     // Screen transition

// Todo
EVT_TODO_CREATED
EVT_TODO_UPDATED
EVT_TODO_DELETED
EVT_TODO_TOGGLED

// Alarm
EVT_ALARM_CREATED
EVT_ALARM_UPDATED
EVT_ALARM_DELETED
EVT_ALARM_TRIGGERED    // Alarm time matched
EVT_ALARM_STOPPED      // Alarm stopped by user

// Schedule
EVT_SCHEDULE_CREATED
EVT_SCHEDULE_UPDATED
EVT_SCHEDULE_DELETED

// System
EVT_TIME_CHANGED
EVT_WIFI_CONNECTED
EVT_WIFI_DISCONNECTED
EVT_DISPLAY_CHANGED
EVT_SOUND_CHANGED
EVT_SYSTEM_ERROR
```

---

## 5. Repository Pattern

### Base Class
```cpp
class Repository {
  virtual bool save(const char* filename, const JsonDocument& doc);
  virtual bool load(const char* filename, JsonDocument& doc);
  virtual bool remove(const char* filename);
};
```

### CRUD Pattern
```cpp
class TodoRepository : public Repository {
  SemaphoreHandle_t _mutex;
  std::vector<Todo> _todos;
public:
  void begin();                    // Load from LittleFS
  std::vector<Todo> getAll();      // Returns copy
  Todo* getById(uint16_t id);      // Direct pointer (fast)
  bool create(const Todo& todo);   // Add + persist
  bool update(const Todo& todo);   // Modify + persist
  bool remove(uint16_t id);        // Delete + persist
};
```

### Mutex Strategy
- Lock on every public method
- Return values by copy (not reference)
- Timeout: 100ms
- No cross-mutex dependencies

---

## 6. Service Pattern

### Example: AlarmService
```cpp
class AlarmService {
  AlarmRepository& _repo;
public:
  void begin();                          // Subscribe to events
  void update(unsigned long ms);         // Check alarms every second
  std::vector<Alarm> getAll();
  Alarm* getById(uint16_t id);
  bool create(const Alarm& alarm);
  bool update(const Alarm& alarm);
  bool remove(uint16_t id);
private:
  void checkAlarms();                    // Internal scheduler
  void emitAlarmEvent(uint16_t id);      // Via EventBus only
};
```

### Key Rule
- Services emit events, never call HAL directly
- Speaker HAL called only by SpeakerService via EventBus

---

## 7. Handler Pattern

### Example: TodoHandlers
```cpp
class TodoHandlers {
public:
  static void handleList(AsyncWebServerRequest* req);
  static void handleGet(AsyncWebServerRequest* req, uint16_t id);
  static void handleCreate(AsyncWebServerRequest* req, AsyncJsonResponse* resp);
  static void handleUpdate(AsyncWebServerRequest* req, uint16_t id, AsyncJsonResponse* resp);
  static void handleDelete(AsyncWebServerRequest* req, uint16_t id);
};
```

### Pattern
1. Parse request (params, body JSON)
2. Call service method
3. Build response JSON
4. Send response

---

## 8. HAL Pattern

### Example: SpeakerHAL
```cpp
class SpeakerHAL {
public:
  void begin();
  void update();
  void alarm(uint8_t pattern, uint8_t volume);
  void stop();
  bool isActive() const;
private:
  void playPattern();
  void updateTone();
  uint32_t _lastToneTime;
  uint8_t _currentPattern;
  bool _active;
};
```

### Key Rules
- No business logic in HAL
- No `delay()` calls
- Non-blocking `update()` pattern
- State machine for complex patterns

---

## 9. TFT Manager

### Screen Router
```cpp
class TFTManager {
public:
  void begin();
  void update(unsigned long ms);        // Called every frame
  void switchScreen(ScreenType screen);
  void invalidate();                    // Force full redraw
private:
  ScreenType _currentScreen;
  bool _dirty;
  unsigned long _lastFrameTime;
  uint32_t _frameCount;
  void renderCurrentScreen();
};
```

### Renderers
- `tftStatusBar` — WiFi/battery/time bar (24px)
- `tftClock` — Clock face (120px area)
- `tftTodo` — Todo list
- `tftSchedule` — Schedule view
- Future: settings, alarm list

### Frame Timing
- Target: 10 FPS (100ms interval)
- Dirty-flag rendering (partial redraw)
- Frame metrics tracked for performance

---

## 10. Navigation State Machine

```
        ┌──────────┐
        │  CLOCK   │ (default)
        └────┬─────┘
             │
        ┌────▼─────┐
        │  MENU    │
        └────┬─────┘
             │
    ┌────────┼────────┐
    │        │        │
┌───▼──┐ ┌──▼───┐ ┌──▼──────┐
│ TODO │ │ ALRM │ │ SCHED   │
└──────┘ └──────┘ └─────────┘
```

### Events
- `EVT_NAV_ENCODER` — Rotate in menu
- `EVT_NAV_SELECT` — Enter screen
- `EVT_NAV_BACK` — Return to clock

---

## 11. Web UI SPA

### Architecture
```
index.html
├── css/
│   ├── reset.css
│   ├── variables.css
│   ├── main.css
│   └── components.css
├── js/
│   ├── core/
│   │   ├── config.js
│   │   ├── router.js
│   │   ├── store.js
│   │   └── app.js
│   ├── api/
│   │   └── client.js (base)
│   ├── components/
│   │   └── 12 components
│   ├── pages/
│   │   └── 9 pages
│   └── utils/
│       ├── dom.js
│       └── helpers.js
└── icons/
    └── sprite.svg
```

### Routing
```javascript
// Hash-based routing
window.addEventListener('hashchange', router.handleRoute);

// Routes
#/dashboard  → DashboardPage
#/todo       → TodoPage
#/alarm      → AlarmPage
#/schedule   → SchedulePage
#/settings   → SettingsPage
```

### Store (Pub/Sub)
```javascript
const Store = {
  state: { ... },
  subscribers: {},
  subscribe(key, callback) { ... },
  set(key, value) {
    this.state[key] = value;
    this.notify(key, value);
  }
};
```

---

## 12. Memory Model

### ESP32 Resources
| Resource | Total | Budget |
|----------|-------|--------|
| Flash | 4MB | Firmware (~431KB) |
| PSRAM | None | N/A |
| RAM | 320KB | ~107KB used |

### Key Allocations
- `DynamicJsonDocument` — Stack-allocated, sized per operation
- `std::vector` — Heap-allocated, returned by value
- `char[]` arrays in models — Predictable memory

---

## 13. Error Handling

### Pattern
```cpp
// Repository: Return bool for success
bool TodoRepository::create(const Todo& todo) {
  if (_todos.size() >= MAX_TODOS) return false;
  // ... persist
  return true;
}

// Service: Emit error events
void TodoService::createFailed(const char* reason) {
  eventBus.emit(EVT_SYSTEM_ERROR, reason);
}

// Handler: Return error response
apiResponse(req, 500, "Failed to create todo");
```

---

## 14. Logging

```cpp
// Levels
logger.debug("TAG", "Detailed info");
logger.info("TAG", "Normal operation");
logger.warn("TAG", "Recoverable issue");
logger.error("TAG", "Unrecoverable error");
logger.fatal("TAG", "System failure");

// Format
logger.info("ALARM", "Triggered alarm %u at %02d:%02d",
  alarm.id, alarm.hour, alarm.minute);
```

### Tag Convention
- 8 characters max
- Uppercase with underscores
- Examples: `TODO_REP`, `ALARM_SV`, `SPEAKER`

---

## 15. Build Configuration

### Arduino IDE Settings
| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Default 4MB with spiffs |
| Upload Speed | 921600 |
| Debug Level | None |

### Libraries
| Library | Version | Purpose |
|---------|---------|---------|
| ESPAsyncWebServer | Latest | HTTP server |
| AsyncTCP | Latest | TCP transport |
| ArduinoJson | v6.x | JSON parsing |
| TFT_eSPI | Latest | Display driver |
| LittleFS | Built-in | Filesystem |

---

*End of Architecture Document*
