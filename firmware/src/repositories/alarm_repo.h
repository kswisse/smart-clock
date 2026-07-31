#ifndef ALARM_REPOSITORY_H
#define ALARM_REPOSITORY_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <mutex>
#include "../models/models.h"
#include "../utils/logger.h"

#define ALARM_FILE "/alarms.json"
#define ALARM_MAX 10
#define REPO_LOCK_TIMEOUT_MS 100

class AlarmRepository {
public:
  bool begin();
  bool load();
  bool save();

  std::vector<Alarm> getAll();
  Alarm getById(uint16_t id);
  Alarm create(const Alarm& alarm);
  bool update(uint16_t id, const Alarm& alarm);
  bool remove(uint16_t id);
  size_t count();

private:
  std::vector<Alarm> _alarms;
  uint16_t _nextId;
  SemaphoreHandle_t _mutex;

  uint16_t _generateId();
  void _lock();
  void _unlock();
};

extern AlarmRepository alarmRepo;

#endif // ALARM_REPOSITORY_H
