# ESP32 Smart Desk Clock - Web Application Implementation Plan

## Architecture

Feature-based SPA with hash routing. Single `index.html` entry point. All pages as JS modules with strict lifecycle (init/load/render/destroy). Centralized state management and API client. No page calls fetch() directly.

## File Structure

```
data/
├── index.html                    ← Entry point, loads CSS/JS
├── css/
│   ├── base.css                  ← Reset, variables, typography (~3KB)
│   ├── components.css            ← Reusable UI styles (~8KB)
│   ├── layout.css                ← Grid, nav, responsive (~5KB)
│   └── pages.css                 ← Page-specific styles (~6KB)
├── js/
│   ├── core/
│   │   ├── config.js             ← Constants, endpoints, defaults
│   │   ├── router.js             ← Hash-based routing
│   │   ├── store.js              ← Centralized state + pub/sub
│   │   └── app.js                ← Bootstrap, lifecycle manager
│   ├── api/
│   │   ├── client.js             ← Fetch wrapper, error handling, cache
│   │   ├── todo.js               ← Todo CRUD
│   │   ├── alarm.js              ← Alarm CRUD
│   │   ├── schedule.js           ← Schedule CRUD
│   │   ├── display.js            ← Display settings
│   │   ├── sound.js              ← Sound settings
│   │   ├── wifi.js               ← WiFi management
│   │   ├── time.js               ← Time sync
│   │   └── device.js             ← Device info
│   ├── components/
│   │   ├── modal.js              ← Modal dialog
│   │   ├── toast.js              ← Toast notifications
│   │   ├── dialog.js             ← Confirm dialog
│   │   ├── card.js               ← Card container
│   │   ├── button.js             ← Button variants
│   │   ├── input.js              ← Text/number inputs
│   │   ├── toggle.js             ← Toggle switch
│   │   ├── slider.js             ← Range slider
│   │   ├── select.js             ← Dropdown select
│   │   ├── list.js               ← List with items
│   │   ├── spinner.js            ← Loading spinner
│   │   └── navbar.js             ← Bottom/side navigation
│   ├── pages/
│   │   ├── dashboard.js          ← Dashboard page
│   │   ├── todo.js               ← Todo list page
│   │   ├── alarm.js              ← Alarm page
│   │   ├── schedule.js           ← Schedule page
│   │   ├── clock-settings.js     ← Clock settings page
│   │   ├── display-settings.js   ← Display settings page
│   │   ├── sound-settings.js     ← Sound settings page
│   │   ├── wifi.js               ← WiFi settings page
│   │   └── device.js             ← Device info page
│   ├── state/
│   │   └── cache.js              ← Client-side cache layer
│   └── utils/
│       ├── helpers.js            ← Debounce, throttle, format
│       └── dom.js                ← DOM helpers, $ selectors
└── icons/
    └── icons.svg                 ← SVG sprite (inline)
```

## Data Models

```javascript
// Todo
{ id: number, title: string, description: string, color: string, completed: boolean, created_at: string }

// Alarm
{ id: number, hour: number, minute: number, repeat: number[], enabled: boolean, sound: string, volume: number }

// Schedule
{ id: number, day: number, start: string, end: string, title: string, color: string }

// Display Settings
{ brightness: number, autoDim: boolean, theme: string, timeout: number, animation: boolean }

// Sound Settings
{ alarmVolume: number, alarmSound: string }

// WiFi Status
{ connected: boolean, ssid: string, ip: string, rssi: number, networks: array }

// Device Info
{ firmware: string, chip: string, flash: string, heap: string, mac: string, battery: number }
```

## API Response Schema

```json
{ "success": true, "data": {}, "message": "OK", "timestamp": 1690000000 }
```

## REST API Endpoints

| Method | Endpoint | Body | Description |
|--------|----------|------|-------------|
| GET | `/api/status` | - | Dashboard aggregate |
| GET | `/api/time` | - | Current time |
| POST | `/api/time` | `{time, date, timezone, mode}` | Set time |
| GET | `/api/todo` | - | List todos |
| POST | `/api/todo` | `{title, description, color}` | Create todo |
| PUT | `/api/todo/:id` | `{title, description, color, completed}` | Update todo |
| DELETE | `/api/todo/:id` | - | Delete todo |
| GET | `/api/alarm` | - | List alarms |
| POST | `/api/alarm` | `{hour, minute, repeat[], sound, volume}` | Create alarm |
| PUT | `/api/alarm/:id` | `{enabled, ...}` | Update alarm |
| DELETE | `/api/alarm/:id` | - | Delete alarm |
| GET | `/api/schedule` | - | List schedule |
| POST | `/api/schedule` | `{day, start, end, title, color}` | Create entry |
| PUT | `/api/schedule/:id` | `{...}` | Update entry |
| DELETE | `/api/schedule/:id` | - | Delete entry |
| GET | `/api/display` | - | Get display settings |
| POST | `/api/display` | `{brightness, autoDim, theme, timeout, animation}` | Update display |
| GET | `/api/sound` | - | Get sound settings |
| POST | `/api/sound` | `{alarmVolume, alarmSound}` | Update sound |
| GET | `/api/wifi` | - | Status + scan |
| POST | `/api/wifi` | `{ssid, password}` or `{action:"disconnect"}` | Connect/disconnect |
| GET | `/api/device` | - | Device info |

## State Shape

```javascript
{
  currentPage: 'dashboard',
  loading: false,
  online: navigator.onLine,
  time: { current: '', date: '', timezone: '', mode: 'ntp' },
  todos: [],
  alarms: [],
  schedule: [],
  display: { brightness: 80, autoDim: false, theme: 'dark', timeout: 30, animation: true },
  sound: { alarmVolume: 50, alarmSound: 'default' },
  wifi: { connected: false, ssid: '', ip: '', rssi: 0, networks: [] },
  device: { firmware: '', chip: '', flash: '', heap: '', mac: '', battery: 0 }
}
```

## Implementation Phases

### Phase 1: Foundation
1. **Folder structure + index.html** - Create all directories, index.html with CSS/JS loading
2. **config.js** - All constants, API endpoints, default values
3. **dom.js** - DOM helper utilities ($, $$, createElement, etc.)
4. **helpers.js** - Debounce, throttle, formatDate, generateId
5. **base.css** - CSS reset, variables, typography, base classes

### Phase 2: Core Systems
6. **store.js** - State management with pub/sub pattern
7. **cache.js** - Client-side cache with TTL
8. **client.js** - API client with error handling, timeout, retry, cache integration
9. **router.js** - Hash-based routing with page lifecycle
10. **app.js** - Application bootstrap, page lifecycle manager

### Phase 3: UI Components
11. **components.css** - All component styles
12. **layout.css** - Navigation, grid, responsive styles
13. **spinner.js** - Loading spinner component
14. **toast.js** - Toast notification system
15. **dialog.js** - Confirm dialog component
16. **modal.js** - Modal dialog component
17. **button.js** - Button component with variants
18. **input.js** - Form input components
19. **toggle.js** - Toggle switch component
20. **slider.js** - Range slider component
21. **select.js** - Dropdown select component
22. **card.js** - Card container component
23. **list.js** - List component
24. **navbar.js** - Navigation bar component

### Phase 4: API Layer
25. **api/todo.js** - Todo API module
26. **api/alarm.js** - Alarm API module
27. **api/schedule.js** - Schedule API module
28. **api/time.js** - Time API module
29. **api/display.js** - Display API module
30. **api/sound.js** - Sound API module
31. **api/wifi.js** - WiFi API module
32. **api/device.js** - Device API module

### Phase 5: Pages
33. **dashboard.js** - Dashboard page
34. **todo.js** - Todo list page
35. **alarm.js** - Alarm management page
36. **schedule.js** - Weekly schedule page
37. **clock-settings.js** - Clock settings page
38. **display-settings.js** - Display settings page
39. **sound-settings.js** - Sound settings page
40. **wifi.js** - WiFi settings page
41. **device.js** - Device info page

### Phase 6: Polish
42. **pages.css** - Page-specific styles
43. **icons.svg** - SVG sprite sheet
44. **Integration testing** - Verify all pages work together
45. **Optimization** - Minification, size audit
46. **Documentation** - README, deployment guide

## Verification Per Module

After each module:
1. File created/modified correctly
2. No syntax errors (ESLint if available)
3. Module exports correct API
4. Dependencies satisfied
5. File under 300 lines
6. No placeholder/TODO comments

## Caching Strategy

| Data | TTL | Refresh |
|------|-----|---------|
| Dashboard status | 10s | On tab focus |
| Todos/Alarms/Schedule | 30s | On mutation |
| Device info | 60s | On demand |
| WiFi scan | None | Always fresh |
| Display/Sound | 30s | On mutation |

## Offline Support

- `navigator.onLine` + periodic ping to `/api/status`
- Offline banner when disconnected
- Serve cached data for reads
- Queue mutations, retry on reconnect
- Show "connection lost" toast

## Responsive Breakpoints

- Mobile: < 480px (1 column, bottom nav)
- Tablet: 480-768px (2 columns, bottom nav)
- Desktop: > 768px (3 columns, side nav)

## Performance Targets

- Total size: < 150KB
- First paint: < 500ms
- Page switch: < 100ms
- API calls: < 2s (ESP32 network)
- DOM nodes: < 500 total
