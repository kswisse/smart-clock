// ──────────────────────────────────────────────────────────────
// PIFKID 2026 Smart Desk Clock - ESP32 Firmware
// ──────────────────────────────────────────────────────────────
// Board:    ESP32 Dev Module
// Upload:   Sketch + LittleFS (Tools > ESP32 Sketch Data Upload)
// Libraries:
//   - ESPAsyncWebServer
//   - AsyncTCP
//   - ArduinoJson
// ──────────────────────────────────────────────────────────────

#ifndef SIMULATION

#include "src/core/config.h"
#include "src/utils/logger.h"
#include "src/events/event_bus.h"
#include "src/hal/hal.h"
#include "src/hal/display_hal.h"
#include "src/hal/speaker_hal.h"
#include "src/hal/buttons_hal.h"
#include "src/hal/tft_todo.h"
#include "src/hal/encoder_hal.h"
#include "src/hal/tft_manager.h"
#include "src/hal/tft_status_bar.h"
#include "src/hal/tft_clock.h"
#include "src/navigation/nav_state.h"
#include "src/services/speaker_service.h"
#include "src/repositories/repository.h"
#include "src/repositories/config_repo.h"
#include "src/services/wifi_service.h"
#include "src/services/time_service.h"
#include "src/services/status_service.h"
#include "src/services/todo_service.h"
#include "src/services/alarm_service.h"
#include "src/services/schedule_service.h"
#include "src/hal/tft_schedule.h"
#include "src/server/server.h"

// ── Setup ───────────────────────────────────────────────────
void setup() {
  logger.begin((LogLevel)LOG_LEVEL, 115200);
  logger.info("BOOT", "═══════════════════════════════════");
  logger.info("BOOT", "  PIFKID 2026 Smart Desk Clock");
  logger.info("BOOT", "  Firmware v%s (API %s)", FW_VERSION, API_VERSION);
  logger.info("BOOT", "  Built: %s", FW_BUILD_TIME);
  logger.info("BOOT", "═══════════════════════════════════");

  HAL::init();

  eventBus.begin();

  if (!repository.begin()) {
    logger.error("BOOT", "CRITICAL: LittleFS failed!");
    while (true) delay(1000);
  }

  configRepo.begin();
  displayHAL.init();
  speakerHAL.init();
  buttonsHAL.init();
  tftTodo.init();
  tftSchedule.init();
  encoderHal.init();
  navState.init();
  tftManager.init();
  speakerService.begin();

  // Initialize WiFi (non-blocking)
  wifiService.begin();

  String ssid = configRepo.wifiSSID();
  if (ssid.length() > 0) {
    // Start STA connection (non-blocking)
    wifiService.startConnectSTA(ssid.c_str(), configRepo.wifiPassword().c_str());
  } else {
    // Start AP mode (immediate)
    wifiService.beginAP();
  }

  timeService.beginNTP(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET);
  statusService.begin();
  todoService.begin();
  alarmService.begin();
  scheduleService.begin();

  server.begin();

  logger.info("BOOT", "───────────────────────────────────");
  logger.info("BOOT", "  AP SSID: %s", WIFI_AP_SSID);
  logger.info("BOOT", "  AP IP:   %s", WiFi.softAPIP().toString().c_str());
  logger.info("BOOT", "  Web UI:  http://%s", WiFi.softAPIP().toString().c_str());
  logger.info("BOOT", "───────────────────────────────────");
  logger.logMemory("BOOT");

  HAL::ledOff();
}

// ── Loop ────────────────────────────────────────────────────
void loop() {
  eventBus.processQueue();
  wifiService.handleEvents();
  timeService.update();
  statusService.update();
  alarmService.update();
  buttonsHAL.update();
  encoderHal.update();
  navState.update();
  tftManager.update();
  if (millis() % 30000 < REDRAW_INTERVAL_MS) {
    logger.logMemory("LOOP");
  }
  speakerService.update();
  HAL::ledBlink(1000);
}

#endif // SIMULATION
