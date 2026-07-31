#include "alarm_repo.h"
#include "repository.h"
#include "../core/config.h"

AlarmRepository alarmRepo;

bool AlarmRepository::begin() {
  _mutex = xSemaphoreCreateMutex();
  if (_mutex == NULL) {
    logger.error("ALARM_REPO", "Mutex creation failed");
    return false;
  }
  _nextId = 1;
  return load();
}

bool AlarmRepository::load() {
  _lock();
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  if (!repository.readJson(ALARM_FILE, doc)) {
    logger.warn("ALARM_REPO", "No alarm file, starting empty");
    _alarms.clear();
    _unlock();
    return true;
  }

  _alarms.clear();
  _nextId = doc["nextId"] | 1;
  JsonArray arr = doc["alarms"].as<JsonArray>();

  for (JsonObject obj : arr) {
    Alarm alarm;
    alarm.clear();
    alarm.id = obj["id"];
    alarm.hour = obj["hour"] | 0;
    alarm.minute = obj["minute"] | 0;
    alarm.enabled = obj["enabled"] | true;
    strncpy(alarm.sound, obj["sound"] | "default", sizeof(alarm.sound) - 1);
    alarm.volume = obj["volume"] | 50;

    JsonArray days = obj["repeat"].as<JsonArray>();
    for (int i = 0; i < 7; i++) alarm.repeatDays[i] = false;
    for (int i = 0; i < days.size() && i < 7; i++) {
      int day = days[i];
      if (day >= 0 && day < 7) alarm.repeatDays[day] = true;
    }
    _alarms.push_back(alarm);
  }

  logger.info("ALARM_REPO", "Loaded %d alarms", _alarms.size());
  _unlock();
  return true;
}

bool AlarmRepository::save() {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  doc["nextId"] = _nextId;
  JsonArray arr = doc.createNestedArray("alarms");

  for (const auto& alarm : _alarms) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = alarm.id;
    obj["hour"] = alarm.hour;
    obj["minute"] = alarm.minute;
    obj["enabled"] = alarm.enabled;
    obj["sound"] = alarm.sound;
    obj["volume"] = alarm.volume;
    JsonArray days = obj.createNestedArray("repeat");
    for (int i = 0; i < 7; i++) {
      if (alarm.repeatDays[i]) days.add(i);
    }
  }

  bool ok = repository.writeJson(ALARM_FILE, doc);
  if (ok) logger.debug("ALARM_REPO", "Saved %d alarms", _alarms.size());
  return ok;
}

std::vector<Alarm> AlarmRepository::getAll() {
  _lock();
  std::vector<Alarm> copy = _alarms;
  _unlock();
  return copy;
}

Alarm AlarmRepository::getById(uint16_t id) {
  _lock();
  for (const auto& alarm : _alarms) {
    if (alarm.id == id) {
      Alarm result = alarm;
      _unlock();
      return result;
    }
  }
  _unlock();
  Alarm empty;
  empty.clear();
  return empty;
}

Alarm AlarmRepository::create(const Alarm& alarm) {
  _lock();
  if (_alarms.size() >= ALARM_MAX) {
    logger.warn("ALARM_REPO", "Alarm limit reached (%d)", ALARM_MAX);
    _unlock();
    Alarm empty;
    empty.clear();
    return empty;
  }

  Alarm newAlarm = alarm;
  newAlarm.id = _generateId();
  _alarms.push_back(newAlarm);
  save();
  _unlock();

  logger.info("ALARM_REPO", "Created alarm %d: %02d:%02d", newAlarm.id, newAlarm.hour, newAlarm.minute);
  return newAlarm;
}

bool AlarmRepository::update(uint16_t id, const Alarm& alarm) {
  _lock();
  for (size_t i = 0; i < _alarms.size(); i++) {
    if (_alarms[i].id == id) {
      _alarms[i] = alarm;
      _alarms[i].id = id;
      save();
      _unlock();
      logger.info("ALARM_REPO", "Updated alarm %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

bool AlarmRepository::remove(uint16_t id) {
  _lock();
  for (size_t i = 0; i < _alarms.size(); i++) {
    if (_alarms[i].id == id) {
      _alarms.erase(_alarms.begin() + i);
      save();
      _unlock();
      logger.info("ALARM_REPO", "Deleted alarm %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

size_t AlarmRepository::count() {
  _lock();
  size_t c = _alarms.size();
  _unlock();
  return c;
}

uint16_t AlarmRepository::_generateId() {
  return _nextId++;
}

void AlarmRepository::_lock() {
  xSemaphoreTake(_mutex, pdMS_TO_TICKS(REPO_LOCK_TIMEOUT_MS));
}

void AlarmRepository::_unlock() {
  xSemaphoreGive(_mutex);
}
