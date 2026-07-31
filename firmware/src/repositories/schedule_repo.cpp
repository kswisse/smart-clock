#include "schedule_repo.h"
#include "repository.h"
#include "../core/config.h"

ScheduleRepository scheduleRepo;

bool ScheduleRepository::begin() {
  _mutex = xSemaphoreCreateMutex();
  if (_mutex == NULL) {
    logger.error("SCH_REPO", "Mutex creation failed");
    return false;
  }
  _nextId = 1;
  return load();
}

bool ScheduleRepository::load() {
  _lock();
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  if (!repository.readJson(FILE_SCHEDULE, doc)) {
    logger.warn("SCH_REPO", "No schedule file, starting empty");
    _entries.clear();
    _unlock();
    return true;
  }

  _entries.clear();
  _nextId = doc["nextId"] | 1;
  JsonArray arr = doc["entries"].as<JsonArray>();

  for (JsonObject obj : arr) {
    ScheduleEntry entry;
    entry.clear();
    entry.id = obj["id"] | 0;
    entry.day = obj["day"] | 0;
    strncpy(entry.startTime, obj["start"] | "00:00", sizeof(entry.startTime) - 1);
    strncpy(entry.endTime, obj["end"] | "00:00", sizeof(entry.endTime) - 1);
    strncpy(entry.title, obj["title"] | "", sizeof(entry.title) - 1);
    strncpy(entry.color, obj["color"] | "#4fc3f7", sizeof(entry.color) - 1);
    _entries.push_back(entry);
  }

  logger.info("SCH_REPO", "Loaded %d entries", _entries.size());
  _unlock();
  return true;
}

bool ScheduleRepository::save() {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  doc["nextId"] = _nextId;
  JsonArray arr = doc.createNestedArray("entries");

  for (const auto& entry : _entries) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = entry.id;
    obj["day"] = entry.day;
    obj["start"] = entry.startTime;
    obj["end"] = entry.endTime;
    obj["title"] = entry.title;
    obj["color"] = entry.color;
  }

  bool ok = repository.writeJson(FILE_SCHEDULE, doc);
  if (ok) logger.debug("SCH_REPO", "Saved %d entries", _entries.size());
  return ok;
}

std::vector<ScheduleEntry> ScheduleRepository::getAll() {
  _lock();
  std::vector<ScheduleEntry> copy = _entries;
  _unlock();
  return copy;
}

std::vector<ScheduleEntry> ScheduleRepository::getByDay(uint8_t day) {
  _lock();
  std::vector<ScheduleEntry> result;
  for (const auto& entry : _entries) {
    if (entry.day == day) result.push_back(entry);
  }
  _unlock();
  return result;
}

ScheduleEntry ScheduleRepository::getById(uint16_t id) {
  _lock();
  for (const auto& entry : _entries) {
    if (entry.id == id) {
      ScheduleEntry result = entry;
      _unlock();
      return result;
    }
  }
  _unlock();
  ScheduleEntry empty;
  empty.clear();
  return empty;
}

ScheduleEntry ScheduleRepository::create(const ScheduleEntry& entry) {
  _lock();
  if (_entries.size() >= MAX_SCHEDULE_ENTRIES) {
    logger.warn("SCH_REPO", "Schedule limit reached (%d)", MAX_SCHEDULE_ENTRIES);
    _unlock();
    ScheduleEntry empty;
    empty.clear();
    return empty;
  }

  ScheduleEntry newEntry = entry;
  newEntry.id = _generateId();
  _entries.push_back(newEntry);
  save();
  _unlock();

  logger.info("SCH_REPO", "Created entry %d: %s %s-%s", newEntry.id,
              newEntry.title, newEntry.startTime, newEntry.endTime);
  return newEntry;
}

bool ScheduleRepository::update(uint16_t id, const ScheduleEntry& entry) {
  _lock();
  for (size_t i = 0; i < _entries.size(); i++) {
    if (_entries[i].id == id) {
      _entries[i] = entry;
      _entries[i].id = id;
      save();
      _unlock();
      logger.info("SCH_REPO", "Updated entry %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

bool ScheduleRepository::remove(uint16_t id) {
  _lock();
  for (size_t i = 0; i < _entries.size(); i++) {
    if (_entries[i].id == id) {
      _entries.erase(_entries.begin() + i);
      save();
      _unlock();
      logger.info("SCH_REPO", "Deleted entry %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

size_t ScheduleRepository::count() {
  _lock();
  size_t c = _entries.size();
  _unlock();
  return c;
}

uint16_t ScheduleRepository::_generateId() {
  return _nextId++;
}

void ScheduleRepository::_lock() {
  xSemaphoreTake(_mutex, pdMS_TO_TICKS(REPO_LOCK_TIMEOUT_MS));
}

void ScheduleRepository::_unlock() {
  xSemaphoreGive(_mutex);
}
