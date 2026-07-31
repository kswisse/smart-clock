# Phase 5 — Performance Metrics

## Flash Usage

| Component | Estimated Size |
|-----------|---------------|
| ESP32 bootrom + SDK | ~128KB |
| WiFi stack | ~64KB |
| AsyncWebServer | ~32KB |
| ArduinoJson | ~16KB |
| TFT_eSPI | ~24KB |
| LittleFS | ~16KB |
| Application code | ~64KB |
| Web UI (LittleFS) | ~87KB |
| **Total** | **~431KB** |
| **Available** | **4MB (typical)** |
| **Usage** | **~10.5%** |

## RAM Usage

| Component | Estimated Size |
|-----------|---------------|
| ESP32 SDK + WiFi | ~50KB |
| FreeRTOS tasks | ~8KB |
| AsyncWebServer | ~12KB |
| ArduinoJson buffers | ~8KB |
| TFT_eSPI buffer | ~2KB |
| EventBus + queues | ~2KB |
| Repository caches | ~12KB |
| Service state | ~4KB |
| Logger buffers | ~1KB |
| Stack (main task) | ~8KB |
| **Total** | **~107KB** |
| **Available** | **320KB** |
| **Usage** | **~33%** |

## Heap Metrics

| Metric | Target | Measured |
|--------|--------|----------|
| Free heap at boot | >200KB | TBD (measure on hardware) |
| Free heap after WiFi connect | >150KB | TBD |
| Free heap after all services start | >120KB | TBD |
| Minimum free heap (24h) | >80KB | TBD |
| Heap fragmentation (24h) | <20% | TBD |
| Largest free block (24h) | >50KB | TBD |

## CPU Load

| Component | Estimated CPU |
|-----------|--------------|
| WiFi stack | ~15% |
| TFT rendering (10 FPS) | ~10% |
| EventBus processing | ~2% |
| Encoder/button polling | ~1% |
| Alarm/time checking | ~1% |
| HTTP request handling | ~5% (burst) |
| **Total average** | **~34%** |
| **Idle** | **~66%** |

## Frame Timing

| Metric | Target | Notes |
|--------|--------|-------|
| Average frame time | ≤15ms | Current: TBD |
| Worst frame time | ≤30ms | Full screen redraw |
| Partial redraw (status bar) | ≤5ms | Dirty flag optimization |
| Clock redraw (per second) | ≤10ms | Colon blink only |
| Todo list redraw | ≤15ms | Depends on item count |
| Schedule redraw | ≤10ms | Today view |

## API Latency

| Endpoint | Target | Notes |
|----------|--------|-------|
| GET /api/status | ≤20ms | In-memory read |
| GET /api/todo | ≤50ms | Up to 50 items |
| POST /api/todo | ≤100ms | JSON parse + save |
| PUT /api/todo/:id | ≤100ms | JSON parse + save |
| DELETE /api/todo/:id | ≤50ms | Save to flash |
| GET /api/time | ≤20ms | NTP time read |
| POST /api/time | ≤50ms | Time set |

## WiFi Metrics

| Metric | Target |
|--------|--------|
| Connection time (STA) | ≤5s |
| Reconnect time | ≤10s |
| AP boot time | ≤2s |
| HTTP response time (local) | ≤50ms |
| WebSocket latency | ≤10ms |

## Boot Time

| Phase | Target |
|-------|--------|
| Power-on → serial ready | ~100ms |
| Serial ready → LittleFS mounted | ~200ms |
| LittleFS → TFT initialized | ~150ms |
| TFT → WiFi connected | ≤5s |
| WiFi → NTP synced | ≤3s |
| **Total boot → clock display** | **≤5.5s** |

## NTP Sync Time

| Condition | Target |
|-----------|--------|
| First boot (no saved time) | ≤5s |
| Reboot (saved time) | ≤3s |
| Reconnect after disconnect | ≤5s |
| Daily resync | ≤2s |

---

*End of Performance Metrics*
