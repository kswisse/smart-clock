# MVP Checklist — PIFKID 2026 Smart Desk Clock

**Version:** 1.0
**Date:** 2026-07-28
**Status:** Architecture Complete — Awaiting Hardware Validation

---

## 1. Core Features

### Clock Display
- [ ] Shows current time (HH:MM:SS)
- [ ] Updates every second
- [ ] Shows date (YYYY-MM-DD)
- [ ] Shows day of week
- [ ] AM/PM indicator
- [ ] Screen stays sharp (no flicker)

### WiFi Connectivity
- [ ] Connects to configured WiFi
- [ ] Shows connection status
- [ ] Auto-reconnects on disconnect
- [ ] Fallback AP mode if no WiFi
- [ ] Stores WiFi credentials in config

### Web UI
- [ ] Loads on mobile browser
- [ ] Loads on desktop browser
- [ ] All pages accessible
- [ ] Dark theme renders correctly
- [ ] No external dependencies needed

### Todo Feature
- [ ] Create new todo
- [ ] View todo list
- [ ] Mark todo complete
- [ ] Delete todo
- [ ] Todos persist after reboot

### Alarm Feature
- [ ] Create alarm with time
- [ ] Set repeat days
- [ ] Enable/disable alarm
- [ ] Alarm triggers at correct time
- [ ] Speaker plays alarm sound
- [ ] Can stop alarm from web UI
- [ ] Alarms persist after reboot

### Schedule Feature
- [ ] Create schedule entry
- [ ] Set day and time range
- [ ] View schedule for day
- [ ] Schedule entries persist after reboot

---

## 2. Hardware Features

### TFT Display
- [ ] Screen initializes on boot
- [ ] Shows status bar (WiFi, battery, time)
- [ ] Shows clock face
- [ ] Screen refreshes smoothly
- [ ] Backlight adjustable

### Rotary Encoder
- [ ] Rotation detected
- [ ] Press detected
- [ ] Navigation works (rotate to move)
- [ ] Selection works (press to select)

### Buttons
- [ ] Button 1 press detected
- [ ] Button 2 press detected
- [ ] Debounce works (no double-press)

### Speaker
- [ ] Alarm sounds on trigger
- [ ] Volume adjustable
- [ ] Can be silenced

### Battery Monitor
- [ ] Shows battery level
- [ ] Updates periodically

---

## 3. API Endpoints

### System
- [ ] GET /api/status — Returns system info
- [ ] GET /api/time — Returns current time
- [ ] POST /api/time — Sets manual time

### Todo CRUD
- [ ] GET /api/todo — List all todos
- [ ] POST /api/todo — Create todo
- [ ] GET /api/todo/:id — Get single todo
- [ ] PUT /api/todo/:id — Update todo
- [ ] DELETE /api/todo/:id — Delete todo

### Alarm CRUD
- [ ] GET /api/alarm — List all alarms
- [ ] POST /api/alarm — Create alarm
- [ ] GET /api/alarm/:id — Get single alarm
- [ ] PUT /api/alarm/:id — Update alarm
- [ ] DELETE /api/alarm/:id — Delete alarm

### Schedule CRUD
- [ ] GET /api/schedule — List all entries
- [ ] POST /api/schedule — Create entry
- [ ] GET /api/schedule/:id — Get single entry
- [ ] PUT /api/schedule/:id — Update entry
- [ ] DELETE /api/schedule/:id — Delete entry

---

## 4. Reliability

### Persistence
- [ ] Todos survive reboot
- [ ] Alarms survive reboot
- [ ] Schedule survives reboot
- [ ] WiFi config survives reboot
- [ ] Display settings survive reboot

### Stability
- [ ] No crashes after 24 hours
- [ ] No memory leaks
- [ ] Handles 100+ API calls
- [ ] Handles rapid encoder rotation
- [ ] Handles concurrent requests

### Error Handling
- [ ] Invalid JSON returns 400
- [ ] Missing fields return 400
- [ ] Not found returns 404
- [ ] Server error returns 500
- [ ] WiFi disconnect shows on UI

---

## 5. Performance

### Timing
- [ ] Boot to clock display < 5s
- [ ] WiFi connect < 10s
- [ ] API response < 100ms
- [ ] TFT redraw < 50ms
- [ ] UI page load < 1s

### Memory
- [ ] Free heap > 80KB
- [ ] No heap fragmentation
- [ ] Min free heap > 50KB after 1 hour

---

## 6. User Experience

### Web UI Navigation
- [ ] Hash routing works (#/dashboard, #/todo, etc.)
- [ ] Back button works
- [ ] Page transitions smooth
- [ ] Loading states shown

### Web UI Forms
- [ ] Todo form validates input
- [ ] Alarm form validates time
- [ ] Schedule form validates time range
- [ ] Error messages shown on failure
- [ ] Success feedback shown

### Web UI Display
- [ ] Status bar shows WiFi status
- [ ] Status bar shows battery level
- [ ] Status bar shows current time
- [ ] Dark theme consistent across pages

---

## 7. Build & Deploy

### Compilation
- [ ] Compiles without errors
- [ ] Compiles without warnings
- [ ] Flash usage < 1.5MB
- [ ] RAM usage < 150KB

### Upload
- [ ] Firmware uploads successfully
- [ ] LittleFS uploads successfully
- [ ] Serial monitor shows boot logs

### First Boot
- [ ] Creates config files on first boot
- [ ] Enters AP mode if no WiFi config
- [ ] Can be configured via web UI

---

## 8. Documentation

### Project Docs
- [x] AGENTS.md — Project summary
- [x] docs/AI_CONTEXT.md — AI context
- [x] docs/ARCHITECTURE.md — System design
- [x] docs/API.md — API reference
- [x] docs/HARDWARE.md — Hardware reference
- [x] docs/MVP.md — This checklist
- [x] docs/BUILD.md — Build instructions
- [x] docs/KNOWN_BUGS.md — Known issues
- [x] docs/ROADMAP.md — Future plans

### Code Docs
- [ ] Inline comments for complex logic
- [ ] Function documentation
- [ ] README with quick start

---

## 9. Sign-Off

| Item | Verified By | Date |
|------|-------------|------|
| Clock display | | |
| WiFi connect | | |
| Web UI loads | | |
| Todo CRUD | | |
| Alarm triggers | | |
| Schedule works | | |
| Encoder works | | |
| Buttons work | | |
| Speaker works | | |
| 24-hour stability | | |

**MVP Complete:** ☐ Yes / ☐ No
**Date Verified:** _______________

---

*End of MVP Checklist*
