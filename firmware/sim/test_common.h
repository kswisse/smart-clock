#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#ifdef SIMULATION

#include "../src/events/event_bus.h"
#include "../src/models/models.h"
#include "../src/repositories/todo_repo.h"
#include "../src/repositories/alarm_repo.h"
#include "../src/repositories/schedule_repo.h"
#include "../src/repositories/config_repo.h"
#include "../src/repositories/repository.h"
#include "../src/services/todo_service.h"
#include "../src/services/alarm_service.h"
#include "../src/services/schedule_service.h"
#include "../src/services/speaker_service.h"
#include "../src/services/status_service.h"
#include "../src/hal/hal.h"
#include "../src/hal/speaker_hal.h"
#include "../src/utils/logger.h"
#include "mock_hal.h"
#include "mock_services.h"
#include "arduino_stubs.h"

#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <functional>

// ── Test Infrastructure ────────────────────────────────────
struct TestStats {
  int passed;
  int failed;
  int total;
  const char* suiteName;
  std::string failures;
};

static TestStats _currentStats = {0, 0, 0, "", ""};

static void testBeginSuite(const char* name) {
  _currentStats = {0, 0, 0, name, ""};
  printf("\n┌─── %s ───\n", name);
}

static void testEndSuite() {
  printf("└─── %s: %d/%d passed",
         _currentStats.suiteName, _currentStats.passed, _currentStats.total);
  if (_currentStats.failed > 0) {
    printf(" (%d FAILED)", _currentStats.failed);
  }
  printf("\n");
}

static void testAssert(bool cond, const char* label) {
  _currentStats.total++;
  if (cond) {
    _currentStats.passed++;
    printf("  ✓ %s\n", label);
  } else {
    _currentStats.failed++;
    printf("  ✗ FAIL: %s\n", label);
    _currentStats.failures += std::string("  ✗ ") + label + "\n";
  }
}

// ── Boot helpers ───────────────────────────────────────────
static void testBootAll() {
  logger.begin(LOG_INFO, 115200);
  eventBus.begin();
  repository.begin();
  todoRepo.begin();
  alarmRepo.begin();
  scheduleRepo.begin();
  configRepo.begin();
  todoService.begin();
  alarmService.begin();
  scheduleService.begin();
  speakerService.begin();
  wifiService.begin();
  timeService.begin();
  statusService.begin();
  displayHAL.init();
  buttonsHAL.init();
  encoderHal.init();
  speakerHAL.init();
  navState.begin();
  tftManager.init();
}

#endif // SIMULATION
#endif // TEST_COMMON_H
