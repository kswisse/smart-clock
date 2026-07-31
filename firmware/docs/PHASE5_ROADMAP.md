# Phase 5 — Prioritized Production Roadmap

## P0: Critical Bugs

### P0-1: Double Alarm Trigger (TD-1)
**Why:** `alarm_service.cpp:108` calls `speakerHAL.alarm(0)` directly AND emits `EVT_ALARM_TRIGGERED`. `speakerService` also subscribes to the event and calls `speakerHAL.alarm(0)`. Result: alarm sounds twice simultaneously.
**Complexity:** Low (1 line removal)
**Flash/RAM impact:** Negligible
**Performance impact:** Eliminates redundant speakerHAL call
**Fix:** Remove `speakerHAL.alarm(0)` from `alarm_service.cpp:108`. Let `speakerService` handle it via EventBus.

### P0-2: Race Conditions on Shared Data (RC-1, RC-2, RC-3)
**Why:** AsyncWebServer callbacks run on a different FreeRTOS task than `loop()`. No mutex on `todoRepo._todos`, `alarmRepo._alarms`, `eventBus._queue`. Concurrent read/write causes undefined behavior.
**Complexity:** High (requires FreeRTOS mutex/semaphore pattern)
**Flash/RAM impact:** +200 bytes flash, +100 bytes RAM per mutex
**Performance impact:** ~0.1ms per lock/unlock
**Fix:** Add `SemaphoreHandle_t` to repositories and EventBus. Lock in HTTP handlers, unlock after.

### P0-3: WiFi Blocking Loop (BL-1, TD-6)
**Why:** `connectSTA()` blocks `loop()` for up to 10 seconds during WiFi connection. All sensors, display, and input freeze.
**Complexity:** Medium (convert to state machine)
**Flash/RAM impact:** +100 bytes flash
**Performance impact:** Eliminates 10s boot hang
**Fix:** Convert `connectSTA()` to non-blocking state machine with timeout.

---

## P1: Compilation Issues

### P1-1: No Build Configuration (BD-1, BD-2)
**Why:** No `platformio.ini` or `arduino-cli.yaml`. Library versions unpinned. Builds may break with library updates.
**Complexity:** Low
**Flash/RAM impact:** None
**Fix:** Create `platformio.ini` with pinned library versions.

### P1-2: TFT_eSPI User_Setup (BD-3)
**Why:** TFT_eSPI requires a `User_Setup.h` or build flags to define pins. Without it, compilation fails or uses wrong pins.
**Complexity:** Low
**Fix:** Create `User_Setup.h` or add `-D` build flags in platformio.ini.

### P1-3: Duplicate Pin Definitions (DL-1, TD-8)
**Why:** `config.h` re-defines pins already in `pin_config.h`. Compiler warnings or silent overrides.
**Complexity:** Low
**Fix:** Remove duplicate defines from `config.h`, keep only `pin_config.h`.

---

## P2: Hardware Integration

### P2-1: Stub Renderers (TD-2, TD-3)
**Why:** `_renderAlarm()` and `_renderSettings()` are stubs. These screens will show blank or stale content.
**Complexity:** Medium
**Flash/RAM impact:** +500 bytes flash per renderer
**Fix:** Implement alarm list and settings renderers.

### P2-2: Redundant Encoder Debounce (DL-2, TD-5)
**Why:** Both `buttons_hal` and `encoder_hal` debounce `PIN_ENCODER_BTN`. Wastes CPU, potential edge conflicts.
**Complexity:** Low
**Fix:** Remove `PIN_ENCODER_BTN` from `buttons_hal` — let `encoder_hal` own it exclusively.

### P2-3: Blocking playTone() (BL-3)
**Why:** `speakerHAL.playTone()` uses `delay()` — blocks loop for tone duration.
**Complexity:** Low
**Fix:** Remove `playTone()` or make it non-blocking like `alarm()`.

### P2-4: NTP Fallback / RTC (EH-5)
**Why:** No RTC hardware. If NTP fails and power is lost, time resets to epoch.
**Complexity:** High (requires RTC hardware or SNTP persistent storage)
**Fix:** Save last-known time to LittleFS on each sync. Restore on boot.

---

## P3: Performance

### P3-1: EventBus Vector Allocation (EB-1, PB-3, MR-5)
**Why:** `_findSubscribers()` allocates a new `std::vector` on every event dispatch. Causes heap fragmentation.
**Complexity:** Medium
**Fix:** Use a fixed-size array or iterate subscribers in-place without allocation.

### P3-2: EventBus Queue O(n) Dequeue (EB-4, PB-4)
**Why:** `_queue.erase(_queue.begin())` shifts all elements left — O(n) per event.
**Complexity:** Low
**Fix:** Use `std::deque` or circular buffer.

### P3-3: Clock Redraw Optimization (PB-2)
**Why:** Clock face redraws entire 120-pixel area every second even when only the colon blinks.
**Complexity:** Low
**Fix:** Only redraw colon on even seconds, full redraw only on minute change.

### P3-4: Todo List Copy (PB-5)
**Why:** `todoRepo.getAll()` returns `std::vector<Todo>` by value — copies entire list on every call.
**Complexity:** Low
**Fix:** Return `const std::vector<Todo>&` reference.

---

## P4: Reliability

### P4-1: LittleFS Recovery (EH-3)
**Why:** If LittleFS fails, firmware hangs forever in `while(true)`.
**Complexity:** Low
**Fix:** Format LittleFS and retry, or continue with in-memory-only mode.

### P4-2: Watchdog Timer (TD-12)
**Why:** No watchdog — firmware can hang indefinitely without reset.
**Complexity:** Low
**Fix:** Enable Task WDT on loop task, feed in `loop()`.

### P4-3: Error Propagation (EH-1, EH-4)
**Why:** Repository failures silently return empty objects. API handlers don't log parse errors.
**Complexity:** Low
**Fix:** Add error codes to repository methods, log deserialization errors.

### P4-4: ID Wrapping (TD-11)
**Why:** `_nextId` wraps at 65535 without checking for collision with existing IDs.
**Complexity:** Low
**Fix:** Check for collision before assigning, or use monotonic counter.

### P4-5: createdAt Uses millis() (TD-10)
**Why:** `todoRepo.cpp:85` uses `millis()` for `createdAt` — resets on reboot, not wall-clock time.
**Complexity:** Low
**Fix:** Use `timeService.getUnixTime()` if available.

---

## P5: Testing

### P5-1: Unit Tests
**Why:** No tests exist. Changes risk regressions.
**Complexity:** High (requires test framework for ESP32)
**Fix:** Add Unity test framework, write tests for repositories, services, EventBus.

### P5-2: Integration Tests
**Why:** No end-to-end API tests.
**Complexity:** Medium
**Fix:** Add curl-based API test scripts.

### P5-3: Hardware Tests
**Why:** No hardware validation procedures.
**Complexity:** Medium
**Fix:** Create validation checklists (see below).

---

## P6: Documentation

### P6-1: API Documentation
### P6-2: Architecture Guide
### P6-3: Wiring Diagram
### P6-4: User Manual
### P6-5: Deployment Guide

---

## P7: Optimization

### P7-1: Flash Size Optimization
### P7-2: RAM Usage Optimization
### P7-3: Power Consumption Optimization
### P7-4: Code Size Reduction

---

## Implementation Order

| Phase | Task | Est. Effort |
|-------|------|-------------|
| P0 | P0-1: Double alarm fix | 5 min |
| P0 | P0-3: WiFi non-blocking | 2 hours |
| P0 | P0-2: Race condition mutex | 4 hours |
| P1 | P1-1: platformio.ini | 30 min |
| P1 | P1-2: TFT_eSPI setup | 30 min |
| P1 | P1-3: Duplicate pin cleanup | 15 min |
| P2 | P2-2: Encoder debounce cleanup | 15 min |
| P2 | P2-3: Blocking playTone fix | 15 min |
| P2 | P2-1: Stub renderers | 4 hours |
| P3 | P3-4: Todo list reference | 15 min |
| P3 | P3-3: Clock redraw opt | 30 min |
| P3 | P3-1: EventBus vector fix | 2 hours |
| P3 | P3-2: EventBus queue fix | 30 min |
| P4 | P4-1: LittleFS recovery | 30 min |
| P4 | P4-2: Watchdog | 15 min |
| P4 | P4-3: Error propagation | 1 hour |
| P4 | P4-4: ID collision check | 15 min |
| P4 | P4-5: createdAt fix | 15 min |
| P5 | P5-1: Unit tests | 8 hours |
| P5 | P5-2: Integration tests | 4 hours |
| P5 | P5-3: Hardware tests | 4 hours |
| P6 | Documentation | 8 hours |
| P7 | Optimization | 4 hours |

**Total estimated effort:** ~45 hours

---

*End of Roadmap*
