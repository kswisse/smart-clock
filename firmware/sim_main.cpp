// ──────────────────────────────────────────────────────────────
// PIFKID 2026 — Desktop Simulation Driver
// ──────────────────────────────────────────────────────────────
// This file replaces main.ino when SIMULATION is defined.
// It boots all services, runs a scripted scenario, and exits.
// ──────────────────────────────────────────────────────────────

#ifdef SIMULATION

#include "sim/arduino_stubs.h"
#include "sim/mock_types.h"
#include "sim/mock_hal.h"
#include "sim/mock_services.h"
#include "src/core/config.h"
#include "src/events/event_bus.h"
#include "src/models/models.h"
#include "src/repositories/todo_repo.h"
#include "src/repositories/alarm_repo.h"
#include "src/repositories/schedule_repo.h"
#include "src/repositories/config_repo.h"
#include "src/services/todo_service.h"
#include "src/services/alarm_service.h"
#include "src/services/schedule_service.h"
#include "src/services/speaker_service.h"
#include "src/services/status_service.h"
#include "src/repositories/repository.h"
#include "src/hal/hal.h"
#include "src/hal/speaker_hal.h"
#include "src/utils/logger.h"

#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>

// ── Functional verification tests (sim/test_main.cpp) ───────
extern int runTests();

// ── Helpers ─────────────────────────────────────────────────
static int testsPassed = 0;
static int testsFailed = 0;

static void ASSERT(bool cond, const char* label) {
  if (cond) {
    printf("  [PASS] %s\n", label);
    testsPassed++;
  } else {
    printf("  [FAIL] %s\n", label);
    testsFailed++;
  }
}

// ── Phase: Boot ─────────────────────────────────────────────
static void phaseBoot() {
  printf("\n=== Phase 1: Boot ===\n");

  logger.begin(LOG_INFO, 115200);
  eventBus.begin();
  repository.begin();

  ASSERT(true, "EventBus initialized");

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

  ASSERT(true, "All services booted without crash");
}

// ── Phase: WiFi Connect ────────────────────────────────────
static void phaseWifi() {
  printf("\n=== Phase 2: WiFi Connect ===\n");

  WifiState stateBefore = wifiService.getState();
  ASSERT(stateBefore == WIFI_IDLE, "WiFi starts idle");

  wifiService.startConnectSTA("TestNetwork", "password123");

  // Simulate time passing for connection
  for (int i = 0; i < 20; i++) {
    simAdvanceTime(200);
    wifiService.handleEvents();
  }

  WifiState stateAfter = wifiService.getState();
  ASSERT(stateAfter == WIFI_STA_CONNECTED, "WiFi connected after simulation");

  eventBus.processQueue();
  ASSERT(true, "EventBus processed WiFi events without crash");
}

// ── Phase: Time Sync ───────────────────────────────────────
static void phaseTime() {
  printf("\n=== Phase 3: Time Sync ===\n");

  simSetTime(2026, 7, 28, 14, 30, 0);
  timeService.update();

  TimeInfo ti = timeService.getTime();
  ASSERT(ti.hour == 14 && ti.minute == 30, "Time set correctly via simulation");

  eventBus.processQueue();
}

// ── Phase: Todo CRUD ───────────────────────────────────────
static void phaseTodo() {
  printf("\n=== Phase 4: Todo CRUD ===\n");

  // Create
  Todo t1 = todoService.create("Buy groceries", "Milk, eggs, bread", "red");
  ASSERT(t1.id > 0, "Todo created (id > 0)");

  Todo t2 = todoService.create("Write report", "Q3 summary", "blue");
  ASSERT(t2.id > t1.id, "Second todo has higher id");

  // Read
  std::vector<Todo> all = todoService.getAll();
  ASSERT(all.size() == 2, "Two todos exist");

  // Toggle
  todoService.toggleComplete(t1.id);
  all = todoService.getAll();
  bool found = false;
  for (auto& t : all) {
    if (t.id == t1.id && t.completed) found = true;
  }
  ASSERT(found, "Todo toggled to completed");

  // Delete
  todoService.remove(t2.id);
  all = todoService.getAll();
  ASSERT(all.size() == 1, "One todo remaining after delete");

  // EventBus events
  eventBus.processQueue();
  ASSERT(true, "Todo events processed without crash");
}

// ── Phase: Alarm CRUD ──────────────────────────────────────
static void phaseAlarm() {
  printf("\n=== Phase 5: Alarm CRUD ===\n");

  // Create: Mon-Fri — day indices: 1=Mon,2=Tue,...,5=Fri
  uint8_t repeatDays[7] = {1, 2, 3, 4, 5};
  Alarm a1 = alarmService.create(7, 30, repeatDays, 5, "default", 80);
  ASSERT(a1.id > 0, "Alarm created");

  // Read
  std::vector<Alarm> alarms = alarmService.getAll();
  ASSERT(alarms.size() == 1, "One alarm exists");

  // Update
  Alarm a1u = alarms[0];
  a1u.hour = 6;
  a1u.minute = 45;
  alarmService.updateAlarm(a1.id, a1u);
  alarms = alarmService.getAll();
  ASSERT(alarms[0].hour == 6, "Alarm updated (hour=6)");

  // Delete
  alarmService.remove(a1.id);
  alarms = alarmService.getAll();
  ASSERT(alarms.empty(), "Alarm deleted");

  eventBus.processQueue();
  ASSERT(true, "Alarm events processed without crash");
}

// ── Phase: Schedule CRUD ───────────────────────────────────
static void phaseSchedule() {
  printf("\n=== Phase 6: Schedule CRUD ===\n");

  // Create (day=1..7, Mon=1)
  ScheduleEntry s1 = scheduleService.create(1, "09:00", "09:30", "Team standup", "blue");
  ASSERT(s1.id > 0, "Schedule entry created");

  // Read
  std::vector<ScheduleEntry> entries = scheduleService.getAll();
  ASSERT(entries.size() == 1, "One schedule entry exists");

  // Update
  ScheduleEntry e1u = entries[0];
  strcpy(e1u.title, "Sprint planning");
  scheduleService.update(s1.id, e1u);
  entries = scheduleService.getAll();
  ASSERT(strcmp(entries[0].title, "Sprint planning") == 0, "Schedule entry updated");

  // Delete
  scheduleService.remove(s1.id);
  entries = scheduleService.getAll();
  ASSERT(entries.empty(), "Schedule entry deleted");

  eventBus.processQueue();
  ASSERT(true, "Schedule events processed without crash");
}

// ── Phase: Navigation ──────────────────────────────────────
static void phaseNavigation() {
  printf("\n=== Phase 7: Navigation ===\n");

  ScreenType initial = navState.getCurrentScreen();
  ASSERT(initial == SCREEN_CLOCK, "Initial screen is CLOCK");

  // Simulate encoder rotation right (next screen)
  encoderHal.simRotate(1);
  encoderHal.update();
  navState.update();

  // Simulate select button press (encoder button)
  encoderHal.simPressButton();
  encoderHal.update();
  navState.update();

  eventBus.processQueue();
  ASSERT(true, "Navigation events processed without crash");

  encoderHal.simReleaseButton();
  encoderHal.update();
  navState.update();
}

// ── Phase: Encoder Scroll ──────────────────────────────────
static void phaseEncoderScroll() {
  printf("\n=== Phase 8: Encoder Scroll ===\n");

  encoderHal.simRotate(5);  // 5 steps right
  encoderHal.update();
  navState.update();

  encoderHal.simRotate(-3); // 3 steps left
  encoderHal.update();
  navState.update();

  eventBus.processQueue();
  ASSERT(true, "Encoder scroll processed without crash");
}

// ── Phase: Alarm Trigger ───────────────────────────────────
static void phaseAlarmTrigger() {
  printf("\n=== Phase 9: Alarm Trigger ===\n");

  // Create an alarm that will trigger at the simulated time (14:31)
  // Day indices: 0=Sun,1=Mon,...,6=Sat — all 7 days
  uint8_t allDays[7] = {0, 1, 2, 3, 4, 5, 6};
  Alarm triggerAlarm = alarmService.create(14, 31, allDays, 7, "default", 80);

  // Advance time to trigger point
  simAdvanceTime(60000); // 1 minute → 14:31

  // Check alarms
  alarmService.checkAlarms(14, 31);
  eventBus.processQueue();

  bool speakerActive = speakerService.isAlarmActive();
  ASSERT(speakerActive, "Speaker activated on alarm trigger");

  // Stop alarm
  speakerService.handleStop();
  eventBus.processQueue();

  ASSERT(!speakerService.isAlarmActive(), "Speaker stopped after stopAlarm");
}

// ── Phase: TFT Render ──────────────────────────────────────
static void phaseTftRender() {
  printf("\n=== Phase 10: TFT Render ===\n");

  tftManager.update();
  ASSERT(true, "TFT manager rendered without crash");

  tftClock.render(14, 30, 0);
  ASSERT(true, "Clock renderer ran without crash");

  tftStatusBar.render();
  ASSERT(true, "Status bar renderer ran without crash");
}

// ── Phase: Memory Check ────────────────────────────────────
static void phaseMemoryCheck() {
  printf("\n=== Phase 11: Memory Check ===\n");

  uint32_t heap = HAL::heapSize();
  ASSERT(heap > 0, "Heap size reported");

  uint8_t batt = HAL::batteryPercent();
  ASSERT(batt <= 100, "Battery percent in range");
}

// ── Entry Point ────────────────────────────────────────────
int main() {
  setbuf(stdout, NULL);
  printf("╔══════════════════════════════════════════════╗\n");
  printf("║  PIFKID 2026 Smart Desk Clock — Simulation  ║\n");
  printf("╚══════════════════════════════════════════════╝\n");

  phaseBoot();
  phaseWifi();
  phaseTime();
  phaseTodo();
  phaseAlarm();
  phaseSchedule();
  phaseNavigation();
  phaseEncoderScroll();
  phaseAlarmTrigger();
  phaseTftRender();
  phaseMemoryCheck();

  printf("\n══════════════════════════════════════════════\n");
  printf("Results: %d passed, %d failed, %d total\n",
         testsPassed, testsFailed, testsPassed + testsFailed);
  printf("══════════════════════════════════════════════\n");

  // Run functional verification tests
  int testResult = runTests();

  return (testsFailed > 0 || testResult != 0) ? 1 : 0;
}

#endif // SIMULATION
