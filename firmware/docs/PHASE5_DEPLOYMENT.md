# Phase 5 — Deployment Checklists

## Release Checklist

| # | Item | Owner | Status |
|---|------|-------|--------|
| REL-1 | All P0 critical bugs fixed | Dev | |
| REL-2 | All P1 compilation issues resolved | Dev | |
| REL-3 | All P2 hardware integration items complete | Dev | |
| REL-4 | PlatformIO build config created | Dev | |
| REL-5 | TFT_eSPI User_Setup configured | Dev | |
| REL-6 | Code compiles without warnings | Dev | |
| REL-7 | Flash size < 1MB | Dev | |
| REL-8 | RAM usage < 50% at steady state | Dev | |
| REL-9 | 24-hour runtime test passed | QA | |
| REL-10 | WiFi reconnect test passed | QA | |
| REL-11 | Alarm trigger test passed | QA | |
| REL-12 | LittleFS corruption recovery test passed | QA | |
| REL-13 | API endpoints all functional | QA | |
| REL-14 | Web UI loads and functions | QA | |
| REL-15 | Serial logs clean (no errors) | QA | |
| REL-16 | Version number updated | Dev | |
| REL-17 | Changelog updated | Dev | |
| REL-18 | Release notes written | PM | |
| REL-19 | Firmware binary generated (.bin) | Dev | |
| REL-20 | Web UI data package generated (.bin) | Dev | |

---

## Manufacturing Checklist

| # | Item | Station | Status |
|---|------|---------|--------|
| MFG-1 | PCB assembled and inspected | SMT | |
| MFG-2 | ESP32 module soldered | SMT | |
| MFG-3 | TFT display connected | Assembly | |
| MFG-4 | Rotary encoder soldered | Assembly | |
| MFG-5 | Buttons soldered | Assembly | |
| MFG-6 | Speaker soldered | Assembly | |
| MFG-7 | Battery connector installed | Assembly | |
| MFG-8 | Status LED installed | Assembly | |
| MFG-9 | Enclosure assembled | Assembly | |
| MFG-10 | Firmware flashed (serial) | Flash | |
| MFG-11 | LittleFS data uploaded | Flash | |
| MFG-12 | Serial number sticker applied | Labeling | |
| MFG-13 | Packaging complete | Packing | |

---

## Factory Test Checklist

| # | Test | Method | Pass Criteria |
|---|------|--------|---------------|
| FT-1 | Power on | Apply power | LED blinks, serial output starts |
| FT-2 | TFT display | Visual check | Clock face visible, no artifacts |
| FT-3 | Buttons | Press each | Serial log shows press events |
| FT-4 | Encoder | Rotate | Serial log shows position changes |
| FT-5 | Speaker | Trigger alarm | Audible sound at correct frequency |
| FT-6 | WiFi AP | Boot without saved WiFi | AP SSID visible on phone |
| FT-7 | WiFi STA | Connect to test WiFi | Connects within 10s |
| FT-8 | Web UI | Open browser | Page loads, all tabs functional |
| FT-9 | Todo CRUD | Create/read/update/delete | All operations succeed |
| FT-10 | Alarm CRUD | Create/read/update/delete | All operations succeed |
| FT-11 | Schedule CRUD | Create/read/update/delete | All operations succeed |
| FT-12 | NTP sync | Wait 10s after WiFi connect | Clock shows correct time |
| FT-13 | Timezone | Set timezone, verify | Time updates correctly |
| FT-14 | Brightness | Adjust via API | Display dims/brightens |
| FT-15 | Battery voltage | Read via API | Voltage within 3.0-4.2V |
| FT-16 | Memory | Check heap via API | Free heap >100KB |
| FT-17 | Filesystem | Create data, reboot, read | Data persists |
| FT-18 | Reboot | Power cycle | Device recovers cleanly |
| FT-19 | Long press | Hold encoder >1s | Back to clock screen |
| FT-20 | Serial log | Check for errors | No ERROR messages |

---

## Acceptance Test Checklist

| # | Test | Method | Pass Criteria |
|---|------|--------|---------------|
| AT-1 | Unboxing | User opens box | All components present |
| AT-2 | First boot | Plug in power | Clock displays time within 30s |
| AT-3 | WiFi setup | Connect phone to AP, enter credentials | Device connects to home WiFi |
| AT-4 | Clock accuracy | Compare with reference clock | ≤2s drift after 24h |
| AT-5 | Todo creation | Create 5 todos via web UI | All appear in list |
| AT-6 | Todo completion | Mark todo as complete | Checkbox toggles correctly |
| AT-7 | Alarm set | Set alarm for 1 minute from now | Alarm triggers at correct time |
| AT-8 | Alarm snooze | Press encoder during alarm | Alarm snoozes for 5 minutes |
| AT-9 | Alarm stop | Long press during alarm | Alarm stops completely |
| AT-10 | Schedule | Add schedule entry for today | Entry appears in today view |
| AT-11 | Brightness | Adjust brightness in settings | Display brightness changes |
| AT-12 | Timezone | Change timezone | Clock time updates |
| AT-13 | Reboot recovery | Unplug and replug | All settings preserved |
| AT-14 | WiFi disconnect/reconnect | Turn router off/on | Device reconnects automatically |
| AT-15 | Display after 1h | Run for 1 hour | No artifacts or flicker |
| AT-16 | Web UI responsiveness | Use on phone browser | All pages load <2s |
| AT-17 | Multiple tabs | Open web UI in 2 browsers | Both work simultaneously |
| AT-18 | Error recovery | Enter invalid data | Graceful error messages |
| AT-19 | Battery indicator | Check status bar | Battery icon shows correct level |
| AT-20 | Overall satisfaction | User evaluation | Meets quality expectations |

---

*End of Deployment Checklists*
