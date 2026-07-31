#ifndef SCHEDULE_REPOSITORY_H
#define SCHEDULE_REPOSITORY_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <mutex>
#include "../models/models.h"
#include "../utils/logger.h"

#define MAX_SCHEDULE_ENTRIES 100
#define REPO_LOCK_TIMEOUT_MS 100

class ScheduleRepository {
public:
  bool begin();
  bool load();
  bool save();

  std::vector<ScheduleEntry> getAll();
  std::vector<ScheduleEntry> getByDay(uint8_t day);
  ScheduleEntry getById(uint16_t id);
  ScheduleEntry create(const ScheduleEntry& entry);
  bool update(uint16_t id, const ScheduleEntry& entry);
  bool remove(uint16_t id);
  size_t count();

private:
  std::vector<ScheduleEntry> _entries;
  uint16_t _nextId;
  SemaphoreHandle_t _mutex;

  uint16_t _generateId();
  void _lock();
  void _unlock();
};

extern ScheduleRepository scheduleRepo;

#endif // SCHEDULE_REPOSITORY_H
