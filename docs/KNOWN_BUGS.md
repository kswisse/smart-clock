# Known Bugs — PIFKID 2026 Smart Desk Clock

**Last Updated:** 2026-07-28
**Status:** Awaiting Hardware Validation

---

## 1. Critical Bugs (P0)

**None reported yet.**

Awaiting hardware bring-up to discover critical issues.

---

## 2. High Priority Bugs (P1)

**None reported yet.**

---

## 3. Medium Priority Bugs (P2)

**None reported yet.**

---

## 4. Low Priority Bugs (P3)

**None reported yet.**

---

## 5. Fixed Bugs

### Fixed in MVP v1.0

| ID | Issue | Fix | Commit |
|----|-------|-----|--------|
| FIX-1 | Race conditions on repositories | FreeRTOS mutexes on all repos + EventBus | P0_2_VERIFICATION.md |
| FIX-2 | WiFi blocks main loop | Non-blocking state machine with exponential backoff | P0_3_VERIFICATION.md |
| FIX-3 | Double alarm trigger | Removed direct speakerHAL calls, unified via EventBus | P0_1_VERIFICATION.md |

### Pre-MVP Fixes

| ID | Issue | Fix |
|----|-------|-----|
| FIX-4 | Alarm triggers multiple times | Added deduplication guard in alarm_service.cpp |
| FIX-5 | Speaker blocks loop | Changed to non-blocking state machine |
| FIX-6 | ConfigRepo returns references | Changed to value returns for thread safety |

---

## 6. Bug Report Template

When reporting a bug, use this format:

```
## Bug Report

**Component:** [Todo/Alarm/Schedule/WiFi/TFT/Encoder/Buttons/Speaker/API]

**Steps to Reproduce:**
1. [Step 1]
2. [Step 2]
3. [Step 3]

**Expected Behavior:**
[What should happen]

**Actual Behavior:**
[What actually happens]

**Serial Output:**
```
[Paste serial output here]
```

**Environment:**
- Firmware Version: [e.g., 1.0.0]
- Hardware: [e.g., ESP32 DevKit V1]
- WiFi: [e.g., Connected to MyWiFi]

**Screenshots:**
[If applicable]

**Additional Notes:**
[Any other relevant information]
```

---

## 7. Known Limitations (Not Bugs)

These are expected behaviors, not bugs:

| Limitation | Reason |
|------------|--------|
| No RTC hardware | Time resets on power loss (NTP recovers) |
| No OTA updates | Must use serial for firmware updates |
| No API authentication | Local network only |
| No persistent snooze | Resets on reboot |
| Settings page stub | Not yet implemented |
| Alarm list stub | Not yet implemented |
| WiFi credentials in config.json | Not in secure storage |

---

## 8. Issue Tracking

### File Location
`docs/KNOWN_BUGS.md`

### Update Process
1. Discover bug during testing
2. Add to appropriate priority section
3. Assign ID (BUG-XXX)
4. Document reproduction steps
5. Fix and verify
6. Move to "Fixed" section

---

*End of Known Bugs*
