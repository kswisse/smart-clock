#ifdef SIMULATION

#include "../test_common.h"

static int _triggeredCount = 0;
static int _stoppedCount = 0;
static uint16_t _lastTriggerId = 0;

static void resetAlarmCounters() {
  _triggeredCount = 0;
  _stoppedCount = 0;
  _lastTriggerId = 0;
}

void testAlarm() {
  testBeginSuite("4. Alarm CRUD + Trigger");

  // Clean up state from previous phases
  {
    std::vector<Alarm> existing = alarmService.getAll();
    for (auto& a : existing) alarmService.remove(a.id);
    eventBus.processQueue();
  }

  resetAlarmCounters();
  eventBus.subscribe(EVT_ALARM_TRIGGERED, [](const Event& e) {
    _triggeredCount++;
    _lastTriggerId = e.itemId;
  });
  eventBus.subscribe(EVT_ALARM_STOPPED, [](const Event& e) { _stoppedCount++; });

  // Create — day indices: 0=Sun,1=Mon,...,6=Sat
  uint8_t allDays[7] = {0,1,2,3,4,5,6};
  Alarm a1 = alarmService.create(8, 0, allDays, 7, "default", 75);
  testAssert(a1.id > 0, "Alarm created with valid id");
  testAssert(a1.hour == 8, "Alarm hour correct");
  testAssert(a1.minute == 0, "Alarm minute correct");
  testAssert(a1.volume == 75, "Alarm volume correct");
  testAssert(a1.enabled == true, "Alarm enabled by default");
  eventBus.processQueue();
  testAssert(_triggeredCount == 0, "No trigger on create");

  // Read
  std::vector<Alarm> alarms = alarmService.getAll();
  testAssert(alarms.size() == 1, "One alarm exists");

  Alarm fetched = alarmService.getById(a1.id);
  testAssert(fetched.id == a1.id, "getById returns correct alarm");
  testAssert(fetched.hour == 8, "getById correct hour");
  testAssert(fetched.minute == 0, "getById correct minute");

  // Update enabled
  testAssert(alarmService.update(a1.id, false), "Disable alarm");
  fetched = alarmService.getById(a1.id);
  testAssert(fetched.enabled == false, "Alarm disabled after update");

  // Update full
  a1.hour = 9;
  a1.minute = 30;
  testAssert(alarmService.updateAlarm(a1.id, a1), "Update alarm fields");
  fetched = alarmService.getById(a1.id);
  testAssert(fetched.hour == 9, "Alarm hour updated");
  testAssert(fetched.minute == 30, "Alarm minute updated");

  // Trigger at correct time
  resetAlarmCounters();
  alarmService.update(a1.id, true);  // re-enable
  alarmService.checkAlarms(0, 0);  // Clear stale trigger state from prior phases
  eventBus.processQueue();
  alarmService.checkAlarms(9, 30);
  eventBus.processQueue();
  testAssert(_triggeredCount == 1, "EVT_ALARM_TRIGGERED emitted once at correct time");
  testAssert(_lastTriggerId == a1.id, "Triggered alarm id correct");

  // Trigger is idempotent (same minute, same alarm)
  _triggeredCount = 0;
  alarmService.checkAlarms(9, 30);
  eventBus.processQueue();
  testAssert(_triggeredCount == 0, "No re-trigger on same minute");

  // Stop triggers EVT_ALARM_STOPPED
  _stoppedCount = 0;
  alarmService.checkAlarms(9, 31);
  eventBus.processQueue();
  testAssert(_stoppedCount == 1, "EVT_ALARM_STOPPED emitted when time passes");

  // Speaker stop works
  speakerService.handleStop();
  testAssert(!speakerService.isAlarmActive(), "Speaker inactive after stop");

  // Snooze works
  alarmService.checkAlarms(9, 30);
  eventBus.processQueue();
  testAssert(speakerService.isAlarmActive(), "Speaker active after re-trigger");
  speakerService.handleSnooze();
  testAssert(!speakerService.isAlarmActive(), "Speaker inactive after snooze");

  // Delete
  size_t before = alarmService.count();
  testAssert(alarmService.remove(a1.id), "Delete alarm returns true");
  testAssert(alarmService.count() == before - 1, "Count decreased after delete");
  eventBus.processQueue();

  // No repeat days = trigger every day
  uint8_t noRepeat[7] = {0,0,0,0,0,0,0};
  Alarm a2 = alarmService.create(12, 0, noRepeat, 0, "default", 50);
  resetAlarmCounters();
  alarmService.checkAlarms(0, 0);  // Clear stale trigger state
  eventBus.processQueue();
  alarmService.checkAlarms(12, 0);
  eventBus.processQueue();
  testAssert(_triggeredCount == 1, "Alarm with no repeat days triggers every day");

  // Wrong time = no trigger
  _triggeredCount = 0;
  alarmService.checkAlarms(12, 1);
  eventBus.processQueue();
  testAssert(_triggeredCount == 0, "No trigger at wrong minute");

  alarmService.remove(a2.id);

  // Persistence
  uint8_t weekdays[7] = {1,2,3,4,5,6,0};  // Mon-Sun
  Alarm a3 = alarmService.create(7, 0, weekdays, 7, "gentle", 60);
  alarmRepo.save();
  alarmRepo.load();
  std::vector<Alarm> reloaded = alarmRepo.getAll();
  bool found = false;
  for (auto& a : reloaded) {
    if (a.id == a3.id && a.hour == 7) found = true;
  }
  testAssert(found, "Alarm survives save/load cycle");

  eventBus.unsubscribe(EVT_ALARM_TRIGGERED);
  eventBus.unsubscribe(EVT_ALARM_STOPPED);
  testEndSuite();
}

#endif
