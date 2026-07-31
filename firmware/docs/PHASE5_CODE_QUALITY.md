# Phase 5 — Code Quality Review

## SOLID Principle Compliance

### S — Single Responsibility
| Module | Rating | Issue |
|--------|--------|-------|
| `tft_manager.cpp` | ⚠️ | Screen router + data fetcher + renderer — three responsibilities |
| `alarm_service.cpp` | ⚠️ | Business logic + direct speaker control + EventBus emission |
| `server.cpp` | ✅ | Clean route setup only |
| `repository.h` | ⚠️ | File I/O + JSON + directory listing |
| `config_repo.cpp` | ✅ | Config persistence only |
| `event_bus.cpp` | ✅ | Event pub/sub only |
| `models.h` | ✅ | Data structures with serialization |

### O — Open/Closed
| Module | Rating | Issue |
|--------|--------|-------|
| `tft_manager.cpp` | ❌ | Adding screens requires modifying switch statement |
| `server.cpp` | ❌ | Adding endpoints requires modifying `_setupRoutes()` |
| `event_bus.cpp` | ✅ | Open for new event types, closed for modification |
| `nav_state.cpp` | ⚠️ | Adding screens requires enum + handler changes |

### L — Liskov Substitution
N/A — no polymorphism used.

### I — Interface Segregation
| Module | Rating | Issue |
|--------|--------|-------|
| `repository.h` | ⚠️ | Large interface — `read`, `write`, `readJson`, `writeJson`, `listDir`, `totalBytes`, `usedBytes` |
| `display_hal.h` | ⚠️ | Many methods (30+) — could split into drawing + text + control interfaces |

### D — Dependency Inversion
| Module | Rating | Issue |
|--------|--------|-------|
| `tft_manager.cpp` | ❌ | Depends on concrete `todoService`, `alarmService`, `scheduleService` |
| `alarm_service.cpp` | ❌ | Depends on concrete `speakerHAL` |
| `handlers/*.cpp` | ❌ | Depend on concrete service classes |

## DRY Violations

| ID | Violation | Fix |
|----|-----------|-----|
| DRY-1 | Pin definitions in `config.h` and `pin_config.h` | Remove from `config.h` |
| DRY-2 | JSON serialization in models and handlers | Use model's `toJson()` in handlers |
| DRY-3 | LittleFS in `repository.h` and `storage.cpp` | Remove `storage.cpp`, use `repository` exclusively |
| DRY-4 | Debounce in `buttons_hal` and `encoder_hal` | Own `PIN_ENCODER_BTN` in `encoder_hal` only |

## KISS Assessment

| Area | Rating | Issue |
|------|--------|-------|
| EventBus | ✅ | Simple, understandable pub/sub |
| Repository | ✅ | Straightforward CRUD |
| Navigation | ✅ | Simple state machine |
| Alarm scheduling | ⚠️ | Complex — time comparison + day-of-week + repeat logic |
| WiFi service | ⚠️ | Blocking connect is simple but problematic |

## YAGNI Assessment

| Feature | Needed? | Notes |
|---------|---------|-------|
| Battery monitoring | ⚠️ | Optional hardware — keep `#ifdef` guards |
| WiFi scanning | ✅ | Needed for setup flow |
| Speaker patterns | ✅ | Three patterns is appropriate |
| Settings page | ⚠️ | Stub — implement only if needed |
| `hexDump()` in logger | ❌ | Never used in production code |

## Memory Safety

| Issue | Severity | Location |
|-------|----------|----------|
| No bounds checking on `strncpy` | Medium | models.h — `strncpy` without null termination guarantee |
| `DynamicJsonDocument` deprecated | Medium | repository.h, todo_handlers.cpp |
| `std::vector` grow/shrink | High | repositories, EventBus — heap fragmentation |
| `String` temporaries | Medium | tft_clock.cpp, config_repo.cpp |
| No stack overflow detection | Low | All modules |

## Thread Safety

| Issue | Severity | Location |
|-------|----------|----------|
| No mutex on repositories | Critical | All repos accessed from HTTP + loop tasks |
| No mutex on EventBus | Critical | emit() from HTTP task, processQueue() from loop |
| No mutex on navState | Medium | read from tft_manager, write from nav_state |
| `millis()` overflow | Low | All modules — wraps at ~49 days |

## Event Ordering

| Issue | Severity | Location |
|-------|----------|----------|
| Events processed FIFO | Low | EventBus — no priority ordering |
| Events during processing queued | Medium | EventBus — new events wait until current batch finishes |
| No event history | Low | EventBus — no debug trace of past events |

## Error Handling

| Pattern | Rating | Issue |
|---------|--------|-------|
| Repository errors | ⚠️ | Return false/empty — caller doesn't always check |
| JSON parse errors | ⚠️ | Return 400 but don't log details |
| WiFi connection errors | ❌ | `connectSTA()` fails but setup continues |
| LittleFS errors | ❌ | Infinite hang on failure |
| Service errors | ⚠️ | Silent failures — no error propagation to UI |

## Logging Quality

| Aspect | Rating | Notes |
|--------|--------|-------|
| Coverage | ⚠️ | Good for init, sparse for runtime errors |
| Levels | ✅ | Proper ERROR/WARN/INFO/DEBUG |
| Tags | ✅ | Consistent 8-char tags |
| Memory logging | ✅ | `logMemory()` available |
| Performance logging | ✅ | Frame time every 60 frames |
| Missing | ⚠️ | No crash logging, no stack traces |

## Code Style Consistency

| Aspect | Rating | Notes |
|--------|--------|-------|
| Naming | ⚠️ | Mix of camelCase and snake_case |
| Comments | ✅ | Minimal, appropriate |
| Indentation | ✅ | Consistent 2-space |
| Header guards | ✅ | Consistent `#ifndef` pattern |
| Include order | ⚠️ | No consistent ordering convention |

---

*End of Code Quality Review*
