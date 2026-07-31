#ifdef SIMULATION

#include "../test_common.h"

void testTime() {
  testBeginSuite("6. Time Service");

  // Manual time set
  simSetTime(2026, 7, 28, 14, 30, 45);
  timeService.update();
  TimeInfo ti = timeService.getTime();
  testAssert(ti.hour == 14, "Manual set: hour correct");
  testAssert(ti.minute == 30, "Manual set: minute correct");
  testAssert(ti.second == 45, "Manual set: second correct");
  testAssert(ti.year == 2026, "Manual set: year correct");
  testAssert(ti.month == 7, "Manual set: month correct");
  testAssert(ti.day == 28, "Manual set: day correct");

  // Time progression
  simAdvanceTime(60000); // +1 minute
  timeService.update();
  ti = timeService.getTime();
  testAssert(ti.hour == 14, "Progression: hour unchanged after 1min");
  testAssert(ti.minute == 31, "Progression: minute advanced by 1");

  simAdvanceTime(3600000); // +1 hour
  timeService.update();
  ti = timeService.getTime();
  testAssert(ti.hour == 15, "Progression: hour advanced by 1");

  // NTP mock sync
  timeService.simSync();
  testAssert(timeService.isNtpSynced(), "NTP sync flag set");
  testAssert(timeService.getTimeSinceSync() == 0, "Time since sync is 0 right after sync");

  simAdvanceTime(5000);
  unsigned long sinceSync = timeService.getTimeSinceSync();
  testAssert(sinceSync >= 5000, "Time since sync increases");

  // Timezone
  timeService.setTimezone("CET-1CEST,M3.5.0,M10.5.0/3");
  String tz = timeService.loadTimezone();
  testAssert(tz.indexOf("CET") >= 0, "Timezone saved and loaded");

  // Time mode
  timeService.setMode("ntp");
  ti = timeService.getTime();
  testAssert(strcmp(ti.mode, "ntp") == 0, "Time mode set to ntp");

  timeService.setMode("manual");
  ti = timeService.getTime();
  testAssert(strcmp(ti.mode, "manual") == 0, "Time mode set to manual");

  testEndSuite();
}

#endif
