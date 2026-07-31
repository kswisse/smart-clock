#ifdef SIMULATION

#include "../test_common.h"

static int _createdCount = 0;

void testSchedule() {
  testBeginSuite("5. Schedule CRUD");

  // Clean up state from previous phases
  {
    std::vector<ScheduleEntry> existing = scheduleService.getAll();
    for (auto& e : existing) scheduleService.remove(e.id);
    eventBus.processQueue();
  }

  _createdCount = 0;
  eventBus.subscribe(EVT_SCHEDULE_CREATED, [](const Event& e) { _createdCount++; });

  // Create
  ScheduleEntry s1 = scheduleService.create(1, "09:00", "09:30", "Standup", "#2196f3");
  testAssert(s1.id > 0, "Schedule entry created with valid id");
  testAssert(s1.day == 1, "Day set correctly");
  testAssert(strcmp(s1.startTime, "09:00") == 0, "Start time correct");
  testAssert(strcmp(s1.endTime, "09:30") == 0, "End time correct");
  testAssert(strcmp(s1.title, "Standup") == 0, "Title correct");
  eventBus.processQueue();
  testAssert(_createdCount == 1, "EVT_SCHEDULE_CREATED emitted");

  ScheduleEntry s2 = scheduleService.create(1, "14:00", "15:00", "Review", "#4caf50");
  ScheduleEntry s3 = scheduleService.create(2, "10:00", "11:00", "Planning", "#ff9800");

  // Read all
  std::vector<ScheduleEntry> all = scheduleService.getAll();
  testAssert(all.size() == 3, "Three entries exist");

  // Query by day
  std::vector<ScheduleEntry> mon = scheduleService.getByDay(1);
  testAssert(mon.size() == 2, "Monday has 2 entries");
  std::vector<ScheduleEntry> tue = scheduleService.getByDay(2);
  testAssert(tue.size() == 1, "Tuesday has 1 entry");
  std::vector<ScheduleEntry> wed = scheduleService.getByDay(3);
  testAssert(wed.size() == 0, "Wednesday has 0 entries");

  // Get by id
  ScheduleEntry fetched = scheduleService.getById(s2.id);
  testAssert(fetched.id == s2.id, "getById returns correct entry");
  testAssert(strcmp(fetched.title, "Review") == 0, "getById returns correct title");

  // Update
  strcpy(s2.title, "Code Review");
  testAssert(scheduleService.update(s2.id, s2), "Update returns true");
  fetched = scheduleService.getById(s2.id);
  testAssert(strcmp(fetched.title, "Code Review") == 0, "Title updated");

  // Delete
  testAssert(scheduleService.remove(s3.id), "Delete returns true");
  all = scheduleService.getAll();
  testAssert(all.size() == 2, "Two entries remain after delete");

  // Count
  testAssert(scheduleService.count() == 2, "count() returns 2");

  // Persistence
  scheduleRepo.save();
  scheduleRepo.load();
  std::vector<ScheduleEntry> reloaded = scheduleRepo.getAll();
  bool found = false;
  for (auto& e : reloaded) {
    if (e.id == s1.id && strcmp(e.title, "Standup") == 0) found = true;
  }
  testAssert(found, "Schedule survives save/load cycle");

  eventBus.unsubscribe(EVT_SCHEDULE_CREATED);
  testEndSuite();
}

#endif
