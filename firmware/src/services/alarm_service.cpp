#include "alarm_service.h"
#include "../utils/logger.h"

AlarmService alarmService;

void AlarmService::begin() {
  alarmRepo.begin();
  _lastCheckMinute = 255;
  for (int i = 0; i < ALARM_MAX; i++) _lastAlarmState[i] = false;
  logger.info("ALARM_SVC", "Service initialized (%d alarms)", alarmRepo.count());
}

void AlarmService::update() {
  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return;

  uint8_t hour = timeinfo.tm_hour;
  uint8_t minute = timeinfo.tm_min;

  // Check every minute
  if (minute != _lastCheckMinute) {
    checkAlarms(hour, minute);
    _lastCheckMinute = minute;
  }
}

std::vector<Alarm> AlarmService::getAll() {
  return alarmRepo.getAll();
}

Alarm AlarmService::getById(uint16_t id) {
  return alarmRepo.getById(id);
}

Alarm AlarmService::create(uint8_t hour, uint8_t minute, const uint8_t* repeatDays,
                           uint8_t repeatCount, const char* sound, uint8_t volume) {
  Alarm alarm;
  alarm.clear();
  alarm.hour = hour;
  alarm.minute = minute;
  alarm.enabled = true;
  strncpy(alarm.sound, sound ? sound : "default", sizeof(alarm.sound) - 1);
  alarm.volume = volume;

  for (int i = 0; i < 7; i++) alarm.repeatDays[i] = false;
  if (repeatDays) {
    for (int i = 0; i < repeatCount && i < 7; i++) {
      if (repeatDays[i] < 7) alarm.repeatDays[repeatDays[i]] = true;
    }
  }

  Alarm created = alarmRepo.create(alarm);
  if (created.id > 0) {
    eventBus.emit(EVT_ALARM_CREATED, created.id);
  }
  return created;
}

bool AlarmService::update(uint16_t id, bool enabled) {
  Alarm existing = alarmRepo.getById(id);
  if (existing.id == 0) return false;

  existing.enabled = enabled;
  bool ok = alarmRepo.update(id, existing);
  if (ok) {
    eventBus.emit(EVT_ALARM_UPDATED, id, enabled ? "enabled" : "disabled");
  }
  return ok;
}

bool AlarmService::updateAlarm(uint16_t id, const Alarm& alarm) {
  bool ok = alarmRepo.update(id, alarm);
  if (ok) {
    eventBus.emit(EVT_ALARM_UPDATED, id);
  }
  return ok;
}

bool AlarmService::remove(uint16_t id) {
  bool ok = alarmRepo.remove(id);
  if (ok) {
    eventBus.emit(EVT_ALARM_DELETED, id);
  }
  return ok;
}

size_t AlarmService::count() {
  return alarmRepo.count();
}

void AlarmService::checkAlarms(uint8_t hour, uint8_t minute) {
  auto alarms = alarmRepo.getAll();
  struct tm timeinfo;
  getLocalTime(&timeinfo, 10);
  uint8_t dayOfWeek = timeinfo.tm_wday;

  for (size_t i = 0; i < alarms.size(); i++) {
    if (!alarms[i].enabled) continue;

    bool shouldRing = _shouldTrigger(alarms[i], hour, minute);
    bool wasRinging = _lastAlarmState[i];

    if (shouldRing && !wasRinging) {
      logger.info("ALARM_SVC", "ALARM TRIGGERED: %02d:%02d (id=%d)",
                  alarms[i].hour, alarms[i].minute, alarms[i].id);
      eventBus.emit(EVT_ALARM_TRIGGERED, alarms[i].id);
    } else if (!shouldRing && wasRinging) {
      eventBus.emit(EVT_ALARM_STOPPED, alarms[i].id);
    }

    _lastAlarmState[i] = shouldRing;
  }
}

bool AlarmService::_shouldTrigger(const Alarm& alarm, uint8_t hour, uint8_t minute) {
  if (alarm.hour != hour || alarm.minute != minute) return false;

  // Check if any repeat day is set
  bool hasRepeat = false;
  for (int i = 0; i < 7; i++) {
    if (alarm.repeatDays[i]) { hasRepeat = true; break; }
  }

  // If no repeat days, trigger every day
  if (!hasRepeat) return true;

  // Check day of week
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10)) return true;
  return alarm.repeatDays[timeinfo.tm_wday];
}

bool AlarmService::_isRepeatDay(const Alarm& alarm, uint8_t dayOfWeek) {
  return alarm.repeatDays[dayOfWeek];
}
