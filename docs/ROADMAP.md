# Roadmap — PIFKID 2026 Smart Desk Clock

**Last Updated:** 2026-07-28

---

## 1. Current Status

**Milestone:** MVP v1.0 — Architecture Complete
**Phase:** Hardware Bring-Up & Validation
**Architecture:** FROZEN

---

## 2. Phase 1-4: Complete ✅

| Phase | Description | Status |
|-------|-------------|--------|
| 1 | Web UI foundation | ✅ Complete |
| 2 | Firmware architecture | ✅ Complete |
| 3 | Todo/Alarm/Schedule features | ✅ Complete |
| 4 | Hardware abstraction (16 tasks) | ✅ Complete |
| 5 | System audit & critical fixes | ✅ Complete |

---

## 3. Phase 5: Hardware Validation 🔄

**Status:** In Progress
**Goal:** Validate every hardware component end-to-end

### Tasks
1. Flash firmware to ESP32
2. Upload LittleFS (web UI)
3. Verify boot sequence
4. Test WiFi connection (AP + STA)
5. Verify TFT display
6. Test encoder navigation
7. Test button inputs
8. Test speaker output
9. Test alarm trigger
10. Test todo CRUD via API
11. Test schedule CRUD via API
12. Run 24-hour stability test

### Deliverables
- Hardware validation checklist (docs/PHASE5_VALIDATION_CHECKLIST.md)
- Stress test procedures (docs/PHASE5_STRESS_TESTS.md)
- Performance metrics (docs/PHASE5_PERFORMANCE_METRICS.md)
- Power consumption estimates (docs/PHASE5_POWER_ESTIMATES.md)

---

## 4. Future Phases (Post-MVP)

**Important:** These are NOT to be implemented yet. Architecture is frozen.

### Phase 6: Code Quality Improvements

| Priority | Task | Complexity | Flash | RAM | Perf Impact |
|----------|------|-----------|-------|-----|-------------|
| P1 | Build config for TFT_eSPI | Low | -5KB | -2KB | Faster boot |
| P2 | Stub renderers (settings, alarm list) | Low | +10KB | +5KB | None |
| P2 | Fix blocking WiFi calls | Medium | Same | Same | +50ms WiFi |
| P3 | EventBus batch processing | Low | -1KB | -2KB | +10% throughput |
| P3 | Clock face partial redraw | Low | Same | Same | -20ms/frame |

### Phase 7: Reliability & Recovery

| Priority | Task | Complexity | Flash | RAM | Perf Impact |
|----------|------|-----------|-------|-----|-------------|
| P4 | LittleFS corruption recovery | Medium | +2KB | +1KB | +100ms boot |
| P4 | Watchdog timer integration | Low | Same | Same | None |
| P4 | Error handler with codes | Low | +1KB | Same | None |
| P5 | Event logging system | Medium | +5KB | +3KB | -5% throughput |
| P5 | Health check endpoint | Low | +1KB | Same | None |

### Phase 8: Frontend Polish

| Priority | Task | Complexity | Flash | RAM | Perf Impact |
|----------|------|-----------|-------|-----|-------------|
| P6 | CSS transitions/animations | Low | +2KB | Same | -10ms/frame |
| P6 | Dark/light theme toggle | Low | +3KB | +1KB | None |
| P6 | Mobile-optimized layout | Medium | +2KB | Same | None |
| P7 | Compressed web assets | Low | -20KB | Same | None |
| P7 | Service worker (offline) | High | +5KB | +3KB | None |

### Phase 9: Advanced Features

| Priority | Task | Complexity | Flash | RAM | Perf Impact |
|----------|------|-----------|-------|-----|-------------|
| P7 | OTA firmware updates | High | +10KB | +5KB | None |
| P7 | API authentication | Medium | +3KB | +2KB | -10ms/request |
| P7 | Brightness auto-dimming | Medium | +1KB | +1KB | None |
| P7 | Deep sleep mode | High | +5KB | +2KB | -99% power |

---

## 5. Implementation Rules

### Before Any Change
1. Explain WHY the change is needed
2. Estimate complexity (Low/Medium/High)
3. Estimate flash impact (KB)
4. Estimate RAM impact (KB)
5. Estimate performance impact
6. Get approval
7. Implement ONE change at a time
8. Compile and verify
9. Produce verification report
10. Commit with clear message

### Priority Levels
| Priority | When to Implement |
|----------|-------------------|
| P1 | After hardware validation |
| P2 | After P1 stable |
| P3 | After P2 stable |
| P4 | After P3 stable |
| P5 | After P4 stable |
| P6 | After P5 stable |
| P7 | Optional/never |

---

## 6. Long-Term Vision

### If Project Continues
1. Add RTC module (DS3231) for time persistence
2. Add OTA updates for easy firmware updates
3. Add MQTT for home automation integration
4. Add multiple clock faces (customizable)
5. Add weather display (WiFi + API)
6. Add pomodoro timer
7. Add music player (SD card)
8. Add ambient light sensor (auto-brightness)
9. Add temperature sensor
10. Add case design (3D printable)

### If Project Ends
1. Document final state
2. Archive repository
3. Write project retrospective
4. Share results (blog, GitHub, etc.)

---

## 7. Milestone Definitions

### MVP v1.0 (Current)
- All core features working
- All hardware components tested
- 24-hour stability verified
- Documentation complete

### v1.1 (Future)
- P1-P3 improvements complete
- No critical bugs
- Performance targets met

### v2.0 (Future)
- Advanced features (P7)
- OTA updates
- API authentication
- Professional finish

---

*End of Roadmap*
