#ifndef SCHEDULE_SERVICE_H
#define SCHEDULE_SERVICE_H

#include <Arduino.h>
#include <vector>
#include "../models/models.h"
#include "../repositories/schedule_repo.h"

class ScheduleService {
public:
  void begin();

  std::vector<ScheduleEntry> getAll();
  std::vector<ScheduleEntry> getByDay(uint8_t day);
  ScheduleEntry getById(uint16_t id);
  ScheduleEntry create(uint8_t day, const char* start, const char* end,
                       const char* title, const char* color);
  bool update(uint16_t id, const ScheduleEntry& entry);
  bool remove(uint16_t id);
  size_t count();
};

extern ScheduleService scheduleService;

#endif // SCHEDULE_SERVICE_H
