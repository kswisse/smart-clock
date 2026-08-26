#include "tft_manager.h"

#ifndef SIMULATION
#include "../core/config.h"
#include "display_hal.h"
#include "tft_status_bar.h"
#include "tft_clock.h"
#include "tft_todo.h"
#include "tft_schedule.h"
#include "../navigation/nav_state.h"
#include "../services/time_service.h"
#include "../services/todo_service.h"
#include "../services/alarm_service.h"
#include "../services/schedule_service.h"
#include "../utils/logger.h"
#include "../events/event_bus.h"

TftManager tftManager;

void TftManager::init() {
  _initialized = true;
  _lastRender = 0;
  _lastFrameTime = 0;
  _avgFrameTime = 0;
  _frameCount = 0;
  _forceFullRedraw = true;

  tftStatusBar.init();
  tftClock.init();
  tftTodo.init();
  tftSchedule.init();

  // Initial full screen clear
  displayHAL.clear();
  tftStatusBar.invalidate();

  // Subscribe to screen change events
  eventBus.subscribe(EVT_SCREEN_CHANGED, [](const Event& e) {
    tftManager.onScreenChanged((ScreenType)e.itemId, (ScreenType)e.itemId);
  });

  // Subscribe to data change events for partial redraw
  eventBus.subscribe(EVT_TODO_CREATED, [](const Event& e) { tftTodo.clear(); });
  eventBus.subscribe(EVT_TODO_UPDATED, [](const Event& e) { tftTodo.clear(); });
  eventBus.subscribe(EVT_TODO_DELETED, [](const Event& e) { tftTodo.clear(); });
  eventBus.subscribe(EVT_TODO_TOGGLED, [](const Event& e) { tftTodo.clear(); });

  eventBus.subscribe(EVT_SCHEDULE_CREATED, [](const Event& e) { tftSchedule.invalidate(); });
  eventBus.subscribe(EVT_SCHEDULE_UPDATED, [](const Event& e) { tftSchedule.invalidate(); });
  eventBus.subscribe(EVT_SCHEDULE_DELETED, [](const Event& e) { tftSchedule.invalidate(); });

  eventBus.subscribe(EVT_ALARM_TRIGGERED, [](const Event& e) {
    tftStatusBar.invalidate();
  });

  eventBus.subscribe(EVT_WIFI_CONNECTED, [](const Event& e) {
    tftStatusBar.updateWifi(true, 0);
  });
  eventBus.subscribe(EVT_WIFI_DISCONNECTED, [](const Event& e) {
    tftStatusBar.updateWifi(false, 0);
  });

  logger.info("TFT_MGR", "EventBus subscriptions active");
}

void TftManager::update() {
  if (!_initialized) return;

  unsigned long now = millis();
  if (now - _lastRender < REDRAW_INTERVAL_MS) return;

  unsigned long frameStart = millis();

  // Status bar always renders
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    tftStatusBar.updateTime(timeStr);
  }
  tftStatusBar.render();

  // Current screen
  renderCurrentScreen();

  // Frame timing
  unsigned long frameTime = millis() - frameStart;
  _lastFrameTime = frameTime;
  _avgFrameTime = (_avgFrameTime * _frameCount + frameTime) / (_frameCount + 1);
  _frameCount++;

  if (_frameCount % 60 == 0) {
    logger.info("PERF", "Frame: avg=%.1fms last=%lums heap=%d",
                _avgFrameTime, _lastFrameTime, ESP.getFreeHeap());
  }

  _lastRender = now;
}

void TftManager::renderCurrentScreen() {
  ScreenType screen = navState.getCurrentScreen();
  switch (screen) {
    case SCREEN_CLOCK:    _renderClock(); break;
    case SCREEN_TODO:     _renderTodo(); break;
    case SCREEN_ALARM:    _renderAlarm(); break;
    case SCREEN_SCHEDULE: _renderSchedule(); break;
    case SCREEN_SETTINGS: _renderSettings(); break;
    default: _renderClock(); break;
  }
}

void TftManager::forceRedraw() {
  _forceFullRedraw = true;
  displayHAL.clear();
  tftStatusBar.invalidate();
  tftClock.invalidate();
  tftTodo.clear();
  tftSchedule.invalidate();
}

void TftManager::onScreenChanged(ScreenType from, ScreenType to) {
  logger.info("TFT_MGR", "Screen: %d -> %d", from, to);
  _forceFullRedraw = true;
  displayHAL.clear();
  tftStatusBar.invalidate();
}

unsigned long TftManager::getLastFrameTime() {
  return _lastFrameTime;
}

float TftManager::getAverageFrameTime() {
  return _avgFrameTime;
}

uint32_t TftManager::getFrameCount() {
  return _frameCount;
}

void TftManager::_renderClock() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return;

  tftClock.render(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  tftClock.renderDate(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
                      timeinfo.tm_mday, timeinfo.tm_wday);
}

void TftManager::_renderTodo() {
  auto todos = todoService.getAll();
  int pending = 0;
  for (const auto& t : todos) {
    if (!t.completed) pending++;
  }
  tftTodo.render(todos, pending);
}

void TftManager::_renderAlarm() {
  // Show alarm list (simplified)
  auto alarms = alarmService.getAll();
  // TODO: Implement alarm list renderer
}

void TftManager::_renderSchedule() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return;
  auto entries = scheduleService.getByDay(timeinfo.tm_wday);
  tftSchedule.renderToday(entries);
}

void TftManager::_renderSettings() {
  displayHAL.printAt(10, CONTENT_Y + 4, "Settings", 2, 0xFFFF, 0x0000);
  // TODO: Implement settings page
}

#endif // SIMULATION
