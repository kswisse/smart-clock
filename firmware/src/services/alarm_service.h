#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <Arduino.h>
#include <vector>
#include "../models/models.h"
#include "../repositories/alarm_repo.h"
#include "../events/event_bus.h"

class AlarmService {
public:
  void begin();
  void update();

  std::vector<Alarm> getAll();
  Alarm getById(uint16_t id);
  Alarm create(uint8_t hour, uint8_t minute, const uint8_t* repeatDays,
               uint8_t repeatCount, const char* sound, uint8_t volume);
  bool update(uint16_t id, bool enabled);
  bool updateAlarm(uint16_t id, const Alarm& alarm);
  bool remove(uint16_t id);
  size_t count();

  // Scheduler
  void checkAlarms(uint8_t hour, uint8_t minute);
  bool isAlarmActive(uint16_t id);

private:
  bool _lastAlarmState[ALARM_MAX];
  uint8_t _lastCheckMinute;

  bool _shouldTrigger(const Alarm& alarm, uint8_t hour, uint8_t minute);
  bool _isRepeatDay(const Alarm& alarm, uint8_t dayOfWeek);
};

extern AlarmService alarmService;

#endif // ALARM_SERVICE_H
