#include "schedule_service.h"
#include "../events/event_bus.h"
#include "../utils/logger.h"

ScheduleService scheduleService;

void ScheduleService::begin() {
  scheduleRepo.begin();
  logger.info("SCH_SVC", "Service initialized (%d entries)", scheduleRepo.count());
}

std::vector<ScheduleEntry> ScheduleService::getAll() {
  return scheduleRepo.getAll();
}

std::vector<ScheduleEntry> ScheduleService::getByDay(uint8_t day) {
  return scheduleRepo.getByDay(day);
}

ScheduleEntry ScheduleService::getById(uint16_t id) {
  return scheduleRepo.getById(id);
}

ScheduleEntry ScheduleService::create(uint8_t day, const char* start, const char* end,
                                      const char* title, const char* color) {
  ScheduleEntry entry;
  entry.clear();
  entry.day = day;
  strncpy(entry.startTime, start ? start : "00:00", sizeof(entry.startTime) - 1);
  strncpy(entry.endTime, end ? end : "00:00", sizeof(entry.endTime) - 1);
  strncpy(entry.title, title ? title : "", sizeof(entry.title) - 1);
  if (color) strncpy(entry.color, color, sizeof(entry.color) - 1);

  ScheduleEntry created = scheduleRepo.create(entry);
  if (created.id > 0) {
    eventBus.emit(EVT_SCHEDULE_CREATED, created.id);
  }
  return created;
}

bool ScheduleService::update(uint16_t id, const ScheduleEntry& entry) {
  bool ok = scheduleRepo.update(id, entry);
  if (ok) {
    eventBus.emit(EVT_SCHEDULE_UPDATED, id);
  }
  return ok;
}

bool ScheduleService::remove(uint16_t id) {
  bool ok = scheduleRepo.remove(id);
  if (ok) {
    eventBus.emit(EVT_SCHEDULE_DELETED, id);
  }
  return ok;
}

size_t ScheduleService::count() {
  return scheduleRepo.count();
}
