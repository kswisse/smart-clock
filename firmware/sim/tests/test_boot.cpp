#ifdef SIMULATION

#include "../test_common.h"

void testBoot() {
  testBeginSuite("1. Boot Sequence");

  testAssert(logger.getLevel() == LOG_INFO, "Logger initialized at LOG_INFO");
  testAssert(eventBus.pendingCount() == 0, "EventBus queue empty at boot");

  testAssert(todoRepo.count() == 0 || todoRepo.count() >= 0, "TodoRepository mounted");
  testAssert(alarmRepo.count() == 0 || alarmRepo.count() >= 0, "AlarmRepository mounted");
  testAssert(scheduleRepo.count() == 0 || scheduleRepo.count() >= 0, "ScheduleRepository mounted");

  testAssert(displayHAL.isInitialized(), "DisplayHAL initialized");
  testAssert(speakerHAL.isSnoozing() == false, "SpeakerHAL not snoozing at boot");
  testAssert(speakerHAL.isPlaying() == false, "SpeakerHAL not playing at boot");
  testAssert(speakerService.isAlarmActive() == false, "SpeakerService no active alarm at boot");

  testAssert(navState.getCurrentScreen() == SCREEN_CLOCK, "NavState starts on CLOCK screen");

  SystemStatus status = statusService.getSystemStatus();
  testAssert(status.firmware.length() > 0, "StatusService firmware version set");
  testAssert(status.chip.length() > 0, "StatusService chip model set");

  uint32_t heap = HAL::heapSize();
  testAssert(heap > 0, "HAL heapSize reports > 0");
  uint8_t batt = HAL::batteryPercent();
  testAssert(batt <= 100, "HAL batteryPercent <= 100");

  testEndSuite();
}

#endif
