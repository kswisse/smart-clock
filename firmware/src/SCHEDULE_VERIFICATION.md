# Schedule Feature Verification Report

## Files Created
| File | Path | Lines |
|------|------|-------|
| schedule_repo.h | `src/repositories/schedule_repo.h` | ~30 |
| schedule_repo.cpp | `src/repositories/schedule_repo.cpp` | ~130 |
| schedule_service.h | `src/services/schedule_service.h` | ~25 |
| schedule_service.cpp | `src/services/schedule_service.cpp` | ~80 |
| schedule_handlers.h | `src/handlers/schedule_handlers.h` | ~25 |
| schedule_handlers.cpp | `src/handlers/schedule_handlers.cpp` | ~140 |
| tft_schedule.h | `src/hal/tft_schedule.h` | ~20 |
| tft_schedule.cpp | `src/hal/tft_schedule.cpp` | ~30 |

## Files Modified
| File | Changes |
|------|---------|
| `server.cpp` | Added Schedule CRUD routes (GET/POST/PUT/DELETE) + CORS OPTIONS |
| `main.ino` | Added `scheduleService.begin()`, `tftSchedule.init()`, includes |

## Architecture Verification

### Vertical Slice
```
ScheduleRepository → ScheduleService → ScheduleHandlers → server.cpp routes → Web frontend
       ↑                                     ↑
  LittleFS persistence                 EventBus events
       ↑
  TftSchedule (hardware stub)
```

### API Endpoints
| Method | Route | Handler | Status |
|--------|-------|---------|--------|
| GET | `/api/schedule` | handleGetAll | OK |
| GET | `/api/schedule?day=0` | handleGetAll (filtered) | OK |
| POST | `/api/schedule` | handleCreate | OK |
| GET | `/api/schedule/:id` | handleGetById | OK |
| PUT | `/api/schedule/:id` | handleUpdate | OK |
| DELETE | `/api/schedule/:id` | handleDelete | OK |
| OPTIONS | `/api/schedule` | sendOptions | OK |

### EventBus Integration
- Create → emits `EVT_SCHEDULE_CREATED`
- Update → emits `EVT_SCHEDULE_UPDATED`
- Delete → emits `EVT_SCHEDULE_DELETED`

### Persistence
- File: `/schedule.json`
- Max entries: `MAX_SCHEDULE_ENTRIES` (100)
- Stores nextId for auto-increment
- JSON fields: id, day, start, end, title, color

### Data Model
```json
{
  "id": 1,
  "day": 1,
  "start": "09:00",
  "end": "10:30",
  "title": "Morning Routine",
  "color": "#4fc3f7"
}
```

### Validation
- Day: 0-6 (validated in handler, returns error for invalid)
- startTime/endTime: string format HH:MM (validated by LCD1602 display logic later)
- Title: max 63 chars (char[64] with null terminator)
- Color: string format #RRGGBB
- Max entries enforced (returns empty entry with id=0)

### TFT Rendering (Stub)
- `renderToday()` — renders today's schedule entries
- `renderWeek()` — renders full week view
- `clear()` — clears schedule display area
- All stubs: no TFT hardware library linked yet

## Issues / Notes
- TFT rendering requires actual TFT_eSPI or similar library integration
- No overlap detection (two entries can overlap in time)
- No recurring schedule support (each entry is day-specific)
- Frontend `schedule.js` needs `?day=N` query param support for filtering
