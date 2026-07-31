# Phase 5 — System Audit Report
## PIFKID 2026 Smart Desk Clock — ESP32 Firmware

**Date:** 2026-07-28
**Auditor:** Lead Embedded Systems Architect
**Scope:** Complete firmware codebase audit

---

## 1. Current Architecture

### Folder Tree
```
firmware/
├── main.ino                          (105 lines) — setup/loop entry point
├── src/
│   ├── core/
│   │   ├── config.h                  (153 lines) — all constants, pin defs, limits
│   │   ├── pin_config.h              (40 lines)  — centralized pin mapping
│   │   ├── firmware_info.h/cpp       — version metadata
│   │   └── storage.h/cpp             (83 lines)  — LittleFS wrapper
│   ├── events/
│   │   └── event_bus.h/cpp           (70+60 lines) — pub/sub event system
│   ├── hal/
│   │   ├── hal.h/cpp                 (106 lines) — legacy HAL (LED, battery, buttons)
│   │   ├── display_hal.h/cpp         (114 lines) — TFT_eSPI driver
│   │   ├── buttons_hal.h/cpp         (89 lines)  — 3-button debounce
│   │   ├── encoder_hal.h/cpp         (112 lines) — rotary encoder + accel
│   │   ├── speaker_hal.h/cpp         (169 lines) — LEDC PWM alarm patterns
│   │   ├── tft_manager.h/cpp         (171 lines) — screen router + frame metrics
│   │   ├── tft_status_bar.h/cpp      (99 lines)  — WiFi/battery/time bar
│   │   ├── tft_clock.h/cpp           (73 lines)  — clock face renderer
│   │   ├── tft_todo.h/cpp            — todo list renderer
│   │   └── tft_schedule.h/cpp        — schedule renderer
│   ├── navigation/
│   │   └── nav_state.h/cpp           (132 lines) — screen navigation FSM
│   ├── models/
│   │   └── models.h                  (190 lines) — Todo, Alarm, ScheduleEntry, DisplaySettings, SoundSettings
│   ├── repositories/
│   │   ├── repository.h/cpp          — LittleFS CRUD base
│   │   ├── todo_repo.h/cpp           (124 lines) — in-memory + JSON persistence
│   │   ├── alarm_repo.h/cpp          — alarm persistence
│   │   ├── schedule_repo.h/cpp       — schedule persistence
│   │   └── config_repo.h/cpp         (125 lines) — device config persistence
│   ├── services/
│   │   ├── todo_service.h/cpp        — todo business logic
│   │   ├── alarm_service.h/cpp       (138 lines) — alarm scheduler + trigger
│   │   ├── schedule_service.h/cpp    — schedule business logic
│   │   ├── speaker_service.h/cpp     — event-driven alarm sounds
│   │   ├── time_service.h/cpp        (200 lines) — NTP + manual time
│   │   ├── wifi_service.h/cpp        (135 lines) — WiFi AP/STA management
│   │   └── status_service.h/cpp      (60 lines)  — system status
│   ├── handlers/
│   │   ├── handlers.h/cpp            — status/time handlers
│   │   ├── todo_handlers.h/cpp       (138 lines) — todo REST API
│   │   ├── alarm_handlers.h/cpp      — alarm REST API
│   │   ├── schedule_handlers.h/cpp   — schedule REST API
│   │   └── api_response.h/cpp        — standardized JSON responses
│   ├── server/
│   │   └── server.h/cpp              (147 lines) — AsyncWebServer route setup
│   └── utils/
│       ├── logger.h/cpp              (128 lines) — serial logging
│       ├── json_util.h/cpp           — JSON helpers
│       └── string_util.h/cpp         — string helpers
├── docs/
│   └── PHASE4_HARDWARE_PLAN.md       — 16-task implementation plan
└── .superpowers/sdd/                 — task briefs and reports
```

**Total production source files:** ~50 files
**Estimated total LoC:** ~4,500 lines

---

## 2. Dependency Graph

### Module Dependencies (Who includes whom)
```
main.ino → config.h, logger.h, event_bus.h, hal.h, display_hal.h, speaker_hal.h,
            buttons_hal.h, encoder_hal.h, tft_manager.h, tft_status_bar.h,
            tft_clock.h, nav_state.h, speaker_service.h, repository.h,
            config_repo.h, wifi_service.h, time_service.h, status_service.h,
            todo_service.h, alarm_service.h, schedule_service.h, tft_schedule.h,
            server.h

tft_manager → display_hal, tft_status_bar, tft_clock, tft_todo, tft_schedule,
              nav_state, time_service, todo_service, alarm_service, schedule_service,
              event_bus, logger

nav_state → encoder_hal, buttons_hal, event_bus, logger

alarm_service → alarm_repo, speaker_hal, event_bus, logger

speaker_service → speaker_hal, event_bus, logger

time_service → config_repo, event_bus, logger

handlers → services (direct calls, no abstraction)
```

### External Library Dependencies
- **ESPAsyncWebServer** — HTTP server
- **AsyncTCP** — TCP transport
- **ArduinoJson** — JSON serialization
- **TFT_eSPI** — TFT display driver
- **LittleFS** — Flash filesystem (built-in)

---

## 3. EventBus Topology

### Event Types (21 total)
```
Navigation:  EVT_NAV_ENCODER, EVT_NAV_SELECT, EVT_NAV_BACK, EVT_SCREEN_CHANGED
Todo:        EVT_TODO_CREATED/UPDATED/DELETED/TOGGLED
Alarm:       EVT_ALARM_CREATED/UPDATED/DELETED/TRIGGERED
Schedule:    EVT_SCHEDULE_CREATED/UPDATED/DELETED
System:      EVT_TIME_CHANGED, EVT_WIFI_CONNECTED, EVT_WIFI_DISCONNECTED,
             EVT_DISPLAY_CHANGED, EVT_SOUND_CHANGED, EVT_SYSTEM_ERROR
```

### Subscribers (wired in tft_manager.cpp and speaker_service.cpp)
```
EVT_SCREEN_CHANGED  → tftManager.onScreenChanged() → clear + invalidate
EVT_TODO_*          → tftTodo.clear()
EVT_SCHEDULE_*      → tftSchedule.invalidate()
EVT_ALARM_TRIGGERED → tftStatusBar.invalidate()
EVT_WIFI_CONNECTED  → tftStatusBar.updateWifi(true)
EVT_WIFI_DISCONNECTED → tftStatusBar.updateWifi(false)
EVT_ALARM_TRIGGERED → speakerService.handleAlarmTrigger()
EVT_SOUND_CHANGED   → speakerService (volume/snooze)
```

### EventBus Issues
| ID | Severity | Issue |
|----|----------|-------|
| EB-1 | **Critical** | `_findSubscribers()` allocates a new `std::vector` on every event dispatch — heap fragmentation risk |
| EB-2 | **High** | `unsubscribe()` removes ALL subscriptions for an event type (not selective) — cannot have multiple independent subscribers for the same event |
| EB-3 | **High** | No reentrancy guard on `emit()` — if a callback emits, the event is queued but `_processing` is true so it will be processed on next `processQueue()` call, which is correct but undocumented |
| EB-4 | **Medium** | `_queue` is `std::vector<Event>` — `erase(begin())` is O(n) for each dequeue. With frequent events this is a performance issue |
| EB-5 | **Medium** | Event::message is `const char*` pointer — caller must ensure the string outlives the event. No ownership semantics |
| EB-6 | **Low** | No event priority — all events processed FIFO in the same loop iteration |

---

## 4. Module Coupling Analysis

### Tight Coupling Issues
| ID | Severity | Issue |
|----|----------|-------|
| CO-1 | **Critical** | `tft_manager.cpp` directly calls `todoService.getAll()`, `alarmService.getAll()`, `scheduleService.getByDay()` — renderer depends on concrete service implementations |
| CO-2 | **High** | `alarm_service.cpp` directly calls `speakerHAL.alarm(0)` AND emits `EVT_ALARM_TRIGGERED` — double-coupling. The speaker service also subscribes to the event and calls `speakerHAL.alarm(0)` — alarm sounds twice |
| CO-3 | **High** | `handlers` directly call `todoService`, `alarmService`, etc. — no abstraction layer for API handlers |
| CO-4 | **Medium** | `nav_state.cpp` directly emits `EVT_SOUND_CHANGED` — navigation module knows about sound system |
| CO-5 | **Medium** | `config.h` contains BOTH pin_config.h includes AND all application constants — single header is a monolith |
| CO-6 | **Low** | Duplicate pin definitions in both `pin_config.h` and `config.h` (PIN_LED_STATUS, PIN_BATTERY_ADC, etc.) |

### Properly Decoupled
- EventBus subscribers (clean pub/sub pattern)
- Repository → Service separation (services don't touch filesystem)
- HAL abstraction (display_hal, encoder_hal, etc.)

---

## 5. Build Dependencies

### Missing Library Declarations
| ID | Severity | Issue |
|----|----------|-------|
| BD-1 | **Critical** | No `platformio.ini` or `arduino-cli.yaml` — build configuration is implicit |
| BD-2 | **High** | No library version pinning — `ESPAsyncWebServer`, `ArduinoJson`, `TFT_eSPI` versions unknown |
| BD-3 | **Medium** | TFT_eSPI `User_Setup.h` configuration not included — pin assignments may conflict with library defaults |
| BD-4 | **Medium** | No `#ifdef` guards for optional hardware (battery ADC, etc.) |

---

## 6. Hardware Dependencies

| Module | Hardware | Pins | Notes |
|--------|----------|------|-------|
| Display | TFT_eSPI (ST7735/ILI9341) | MOSI=23, SCLK=18, CS=5, DC=16, RST=17, BL=4 | SPI, landscape rotation |
| Encoder | Rotary | A=25, B=26, BTN=27 | Quadrature, pullup |
| Buttons | 2x tactile | 32, 33 | Active low, pullup |
| Speaker | Piezo buzzer | 21 | LEDC PWM |
| Battery | ADC | 34 | Voltage divider (optional) |
| Status LED | LED | 2 | Active high |

---

## 7. Technical Debt

| ID | Severity | Description | Location |
|----|----------|-------------|----------|
| TD-1 | **Critical** | `alarm_service.cpp:108` calls `speakerHAL.alarm(0)` DIRECTLY — bypasses speakerService, causes double-trigger | alarm_service.cpp:108 |
| TD-2 | **Critical** | `_renderAlarm()` in tft_manager.cpp is a stub with `// TODO: Implement alarm list renderer` | tft_manager.cpp:155-159 |
| TD-3 | **Critical** | `_renderSettings()` in tft_manager.cpp is a stub with `// TODO: Implement settings page` | tft_manager.cpp:168-170 |
| TD-4 | **High** | `tft_clock.cpp:56-66` creates temporary `String` objects for each digit — unnecessary heap allocation in hot path | tft_clock.cpp:56-66 |
| TD-5 | **High** | `buttons_hal.cpp` and `encoder_hal.cpp` both debounce `PIN_ENCODER_BTN` — redundant processing | buttons_hal.cpp:9, encoder_hal.cpp:56 |
| TD-6 | **High** | `wifi_service.cpp:23` has blocking `while` loop for WiFi connection — blocks entire loop() for up to 10 seconds | wifi_service.cpp:23-29 |
| TD-7 | **High** | `repository.cpp` uses `DynamicJsonDocument` which is deprecated in ArduinoJson v7 | repository.h:22 |
| TD-8 | **Medium** | `config.h` has duplicate pin definitions with `pin_config.h` | config.h:21-28 vs pin_config.h |
| TD-9 | **Medium** | `storage.cpp` uses `Serial.printf` directly instead of `logger` | storage.cpp:8-12 |
| TD-10 | **Medium** | `todo_repo.cpp:85` uses `millis()` for `createdAt` — not wall-clock time, resets on reboot | todo_repo.cpp:85 |
| TD-11 | **Medium** | `_nextId` in repositories wraps at 65535 without collision check | todo_repo.cpp:123 |
| TD-12 | **Low** | No watchdog timer configuration | main.ino |

---

## 8. Code Smells

| ID | Severity | Smell | Location |
|----|----------|-------|----------|
| CS-1 | **High** | **Feature Envy** — tft_manager.cpp knows too much about todoService/alarmService/scheduleService internals | tft_manager.cpp:146-165 |
| CS-2 | **High** | **Shotgun Surgery** — adding a new screen requires changes in nav_state.h (enum), tft_manager.cpp (render switch), tft_manager.cpp (EventBus subscriptions), main.ino (includes) | Multiple |
| CS-3 | **Medium** | **Long Method** — `server._setupRoutes()` is 120 lines with no decomposition | server.cpp:27-147 |
| CS-4 | **Medium** | **Primitive Obsession** — alarm repeatDays stored as `bool[7]` instead of bitmask | models.h:49 |
| CS-5 | **Medium** | **Magic Numbers** — `255` for _lastCheckMinute init, `10` for getLocalTime timeout | alarm_service.cpp:9, multiple |
| CS-6 | **Low** | **Dead Code** — `HAL::update()` is empty, `Server::handleClient()` is empty | hal.cpp:30, server.cpp:20 |
| CS-7 | **Low** | **Inconsistent Naming** — mix of `camelCase` and `snake_case` across modules | Multiple |

---

## 9. Duplicate Logic

| ID | Severity | Description |
|----|----------|-------------|
| DL-1 | **High** | Pin definitions duplicated in `config.h` and `pin_config.h` |
| DL-2 | **High** | `buttons_hal.cpp` and `encoder_hal.cpp` both debounce `PIN_ENCODER_BTN` |
| DL-3 | **Medium** | `repository.h` and `storage.h` both provide LittleFS file I/O — two abstractions for same thing |
| DL-4 | **Medium** | JSON serialization duplicated in handlers (`_todoToJson`) and models (`toJson`) |
| DL-5 | **Low** | `File::close()` called manually instead of using RAII or scope guard |

---

## 10. Potential Race Conditions

| ID | Severity | Issue |
|----|----------|-------|
| RC-1 | **Critical** | AsyncWebServer callbacks run on a different FreeRTOS task than `loop()` — services accessed from both tasks without mutex |
| RC-2 | **High** | `todoRepo._todos` vector modified by HTTP handlers (create/update/delete) while `tftManager._renderTodo()` reads it — no synchronization |
| RC-3 | **High** | `eventBus._queue` modified by `emit()` (from HTTP task) and `processQueue()` (from loop task) — no mutex |
| RC-4 | **Medium** | `alarmService.checkAlarms()` reads alarm repo while HTTP handler may modify it |
| RC-5 | **Medium** | `navState._currentScreen` read by tft_manager from loop task, written from nav_state which also runs in loop — safe for now but fragile |

---

## 11. Memory Risks

| ID | Severity | Issue |
|----|----------|-------|
| MR-1 | **Critical** | `std::vector<Todo>` in todoRepo — up to 50 todos × ~200 bytes = ~10KB heap, all allocated dynamically |
| MR-2 | **High** | `DynamicJsonDocument(JSON_DOC_LARGE)` = 2048 bytes allocated on every API call |
| MR-3 | **High** | `std::vector<Subscription>` in EventBus grows unboundedly — each `subscribe()` pushes, never freed |
| MR-4 | **High** | `std::vector<Event>` in EventBus queue grows unboundedly during burst |
| MR-5 | **Medium** | `_findSubscribers()` allocates a new `std::vector` per event dispatch |
| MR-6 | **Medium** | `String` objects in `tft_clock.cpp` digit rendering — temporary allocations |
| MR-7 | **Medium** | `configRepo.wifiSSID()` returns `String` — allocates heap for a copy |
| MR-8 | **Low** | `TodoHandlers::_todosToJson()` uses `DynamicJsonDocument(2048)` for up to 50 todos |

---

## 12. Blocking Operations

| ID | Severity | Operation | Location | Duration |
|----|----------|-----------|----------|----------|
| BL-1 | **Critical** | WiFi `connectSTA()` busy-wait loop | wifi_service.cpp:23-29 | Up to 10 seconds |
| BL-2 | **High** | `getLocalTime(&timeinfo, 10)` blocks for up to 10ms per call | Multiple (tft_manager, alarm_service) | 10ms each |
| BL-3 | **Medium** | `playTone()` uses `delay()` | speaker_hal.cpp:87 | Variable |
| BL-4 | **Medium** | `WiFi.scanNetworks()` blocks | wifi_service.cpp:52 | 1-3 seconds |
| BL-5 | **Low** | `repository.readJson()` / `writeJson()` blocks on flash I/O | repository.cpp | 10-50ms |

---

## 13. Performance Bottlenecks

| ID | Severity | Bottleneck | Impact |
|----|----------|------------|--------|
| PB-1 | **Critical** | Full screen clear on every screen transition (`displayHAL.clear()`) — 240×320×2 = 153,600 bytes written | ~15ms per transition |
| PB-2 | **High** | Clock face redraws entire clock area every second even if only colon blinks | Redundant ~5KB writes |
| PB-3 | **High** | `_findSubscribers()` linear scan + vector allocation per event | O(n) per dispatch |
| PB-4 | **Medium** | EventBus queue `erase(begin())` is O(n) per dequeue | Slow with burst events |
| PB-5 | **Medium** | `todoRepo.getAll()` returns vector by value — copies entire list | ~10KB copy per call |
| PB-6 | **Low** | No dirty-flag optimization in `_renderAlarm()` (stub) | N/A |

---

## 14. SOLID Violations

| Principle | Violation | Location |
|-----------|-----------|----------|
| **S** (Single Responsibility) | `tft_manager.cpp` is both a screen router AND a data fetcher AND a renderer | tft_manager.cpp:146-165 |
| **O** (Open/Closed) | Adding a new screen requires modifying the switch statement in `renderCurrentScreen()` | tft_manager.cpp:97-107 |
| **L** (Liskov Substitution) | N/A — no polymorphism used |
| **I** (Interface Segregation) | `Repository` class mixes file I/O, JSON ops, and directory listing | repository.h |
| **D** (Dependency Inversion) | `tft_manager.cpp` depends on concrete `todoService` instead of an interface | tft_manager.cpp:147 |

---

## 15. DRY Violations

| ID | Violation | Files |
|----|-----------|-------|
| DRY-1 | Pin definitions in two places | config.h, pin_config.h |
| DRY-2 | JSON serialization for Todo in both model and handler | models.h, todo_handlers.cpp |
| DRY-3 | LittleFS access in both `repository.h` and `storage.cpp` | repository.h, storage.cpp |
| DRY-4 | Debounce logic in both `buttons_hal` and `encoder_hal` | buttons_hal.cpp, encoder_hal.cpp |

---

## 16. Error Handling Gaps

| ID | Gap | Location |
|----|-----|----------|
| EH-1 | `todoRepo.create()` silently returns empty Todo on failure — no error propagation | todo_repo.cpp:76-80 |
| EH-2 | `alarmService.checkAlarms()` calls `speakerHAL.alarm()` without checking if speaker is available | alarm_service.cpp:108 |
| EH-3 | No recovery from LittleFS corruption — `while(true) delay(1000)` is infinite hang | main.ino:50 |
| EH-4 | `deserializeJson()` errors in handlers return 400 but don't log the error details | todo_handlers.cpp:35-37 |
| EH-5 | WiFi connection failure in `connectSTA()` returns false but `setup()` continues — no fallback | main.ino:66-68 |
| EH-6 | No null checks on `AsyncWebServerRequest*` in handlers | handlers/*.cpp |

---

## 17. Logging Assessment

| Aspect | Status |
|--------|--------|
| Log levels | ✅ Properly defined (ERROR/WARN/INFO/DEBUG/VERBOSE) |
| Tags | ⚠️ Mostly 8-char, some inconsistent ("TFT_MGR" vs "NAV") |
| Memory logging | ✅ `logMemory()` available |
| Crash logging | ❌ No stack trace or crash reason logging |
| Event logging | ⚠️ Only at DEBUG level — production should log WARN+ |
| Performance logging | ✅ Frame time logged every 60 frames |

---

## 18. Summary Statistics

| Metric | Value |
|--------|-------|
| Total source files | ~50 |
| Total LoC (est.) | ~4,500 |
| External libraries | 5 |
| Pin assignments | 12 |
| Event types | 21 |
| API endpoints | 15 (5 resources × 3 methods + OPTIONS) |
| Model structs | 5 |
| Repository classes | 5 |
| Service classes | 7 |
| HAL modules | 8 |
| Handler classes | 4 |
| Critical issues | 7 |
| High issues | 12 |
| Medium issues | 11 |
| Low issues | 7 |

---

*End of Audit Report*
