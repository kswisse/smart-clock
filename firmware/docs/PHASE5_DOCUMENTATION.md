# Phase 5 — Documentation Requirements

## User Manual
**Audience:** End users (non-technical)
**Content:**
- Device overview and features
- Initial setup (WiFi configuration via AP mode)
- Clock display usage
- Todo management (via web UI)
- Alarm configuration
- Schedule management
- Settings (brightness, sound, timezone)
- Troubleshooting (LED indicators, reset procedure)
- Safety and regulatory compliance

## Technical Manual
**Audience:** Engineers and developers
**Content:**
- System architecture overview
- Hardware specifications
- Pin assignments and connections
- Firmware update procedure
- API reference (all endpoints)
- Configuration parameters
- Debugging procedures (serial log levels)
- Performance characteristics
- Known limitations

## Architecture Guide
**Audience:** Developers
**Content:**
- Module hierarchy (HAL → Services → Handlers → UI)
- EventBus topology and event flow
- Data flow diagrams (API → Service → Repository → LittleFS)
- Screen navigation state machine
- Thread safety model (FreeRTOS tasks)
- Memory management strategy
- Error handling patterns

## API Documentation
**Audience:** Web developers
**Content:**
- Base URL and authentication (none)
- Response format (`{success, code, message, data, timestamp}`)
- Endpoint reference:
  - GET/POST `/api/status`
  - GET/POST `/api/time`
  - GET/POST/PUT/DELETE `/api/todo`
  - GET/POST/PUT/DELETE `/api/alarm`
  - GET/POST/PUT/DELETE `/api/schedule`
- Error codes and messages
- Rate limiting (none currently)
- CORS policy

## Wiring Diagram
**Audience:** Hardware assemblers
**Content:**
- ESP32 DevKit pinout
- TFT display connections (SPI)
- Rotary encoder connections
- Button connections
- Speaker connection
- Battery voltage divider
- Status LED
- Power supply requirements

## Module Diagram
**Audience:** Developers
**Content:**
- Component relationships (box-and-arrow)
- Dependency direction (unidirectional)
- Interface definitions (method signatures)
- Global instances (singletons)

## Sequence Diagrams
**Audience:** Developers
**Content:**
- Boot sequence (setup → init → connect → serve)
- API request handling (request → handler → service → repo → response)
- Alarm trigger flow (time match → EventBus → speaker)
- Screen navigation (encoder → nav_state → tft_manager → renderer)
- WiFi reconnect flow (disconnect → detect → reconnect → notify)

## State Machine Diagram
**Audience:** Developers
**Content:**
- Navigation states (CLOCK → TODO → ALARM → SCHEDULE → SETTINGS)
- WiFi states (DISCONNECTED → CONNECTING → CONNECTED → RECONNECTING)
- Speaker states (IDLE → PLAYING → SNOOZING → STOPPED)
- Alarm states (DISABLED → ARMED → TRIGGERED → SNOOZED → DISMISSED)

## Deployment Guide
**Audience:** Deployers
**Content:**
- Prerequisites (Arduino IDE, libraries, board package)
- Build configuration (platformio.ini or manual setup)
- Upload firmware via serial
- Upload LittleFS data via tool
- First boot procedure
- WiFi configuration
- Firmware update procedure (OTA or serial)
- Rollback procedure

## Maintenance Guide
**Audience:** Maintainers
**Content:**
- Log analysis (serial output patterns)
- Common failure modes and recovery
- Filesystem maintenance (format, backup)
- Battery replacement procedure
- Hardware troubleshooting
- Version management
- Release process

---

*End of Documentation Requirements*
