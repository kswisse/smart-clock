#include "time_service.h"

#ifndef SIMULATION
#include "../core/config.h"
#include "../repositories/config_repo.h"
#include "../utils/logger.h"
#include <ArduinoJson.h>

TimeService timeService;

bool TimeService::beginNTP(const char* server, long gmtOffset, int daylightOffset) {
  logger.info("TIME", "Configuring NTP: %s, GMT%+d", server, gmtOffset / 3600);

  _lastNtpSync = 0;
  _wasSynced = false;
  memset(_savedTimezone, 0, sizeof(_savedTimezone));

  configTime(gmtOffset, daylightOffset, server, NTP_SERVER_2);
  _ntpActive = true;
  _manualMode = false;
  strncpy(_info.mode, "ntp", sizeof(_info.mode));

  String savedTz = loadTimezone();
  if (savedTz.length() > 0) {
    setTimezone(savedTz.c_str());
    logger.info("TIME", "Loaded timezone: %s", savedTz.c_str());
  }

  _refreshTime();
  return true;
}

void TimeService::setManual(int year, int month, int day, int hour, int minute, int second) {
  struct tm t = {0};
  t.tm_year = year - 1900;
  t.tm_mon = month - 1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = -1;

  time_t epoch = mktime(&t);
  struct timeval tv = { .tv_sec = epoch };
  settimeofday(&tv, NULL);

  _ntpActive = false;
  _manualMode = true;
  strncpy(_info.mode, "manual", sizeof(_info.mode));
  _refreshTime();
  logger.info("TIME", "Manual time set: %04d-%02d-%02d %02d:%02d:%02d",
              year, month, day, hour, minute, second);
}

void TimeService::setMode(const char* mode) {
  if (strcmp(mode, "ntp") == 0) {
    _manualMode = false;
    _ntpActive = true;
    strncpy(_info.mode, "ntp", sizeof(_info.mode));
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, NTP_SERVER, NTP_SERVER_2);
    logger.info("TIME", "Mode: NTP");
  } else {
    _manualMode = true;
    _ntpActive = false;
    strncpy(_info.mode, "manual", sizeof(_info.mode));
    logger.info("TIME", "Mode: Manual");
  }
}

void TimeService::setTimezone(const char* tz) {
  strncpy(_timezone, tz, sizeof(_timezone));
  logger.info("TIME", "Timezone: %s", tz);
}

TimeInfo TimeService::getTime() {
  _refreshTime();
  return _info;
}

void TimeService::update() {
  if (millis() - _lastUpdate >= TIME_UPDATE_INTERVAL_MS) {
    _refreshTime();
    _lastUpdate = millis();
  }
}

String TimeService::timeToJson() {
  _refreshTime();
  StaticJsonDocument<256> doc;
  doc["current"] = _info.current;
  doc["date"] = _info.date;
  doc["timezone"] = _info.timezone;
  doc["mode"] = _info.mode;
  doc["hour"] = _info.hour;
  doc["minute"] = _info.minute;
  doc["second"] = _info.second;

  String json;
  serializeJson(doc, json);
  return json;
}

bool TimeService::setTimeFromJson(const char* json) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    logger.error("TIME", "JSON parse error: %s", err.c_str());
    return false;
  }

  const char* mode = doc["mode"] | "ntp";
  const char* tz = doc["timezone"] | "UTC";

  if (strcmp(mode, "manual") == 0) {
    int year = doc["date"] | 2026;
    int month = 1, day = 1, hour = 0, minute = 0;

    // Parse date string if provided
    const char* dateStr = doc["date"] | "";
    if (strlen(dateStr) >= 10) {
      sscanf(dateStr, "%d-%d-%d", &year, &month, &day);
    }

    // Parse time string if provided
    const char* timeStr = doc["time"] | "";
    if (strlen(timeStr) >= 5) {
      sscanf(timeStr, "%d:%d", &hour, &minute);
    }

    // Also check direct fields
    if (doc.containsKey("hour")) hour = doc["hour"];
    if (doc.containsKey("minute")) minute = doc["minute"];

    setManual(year, month, day, hour, minute, 0);
  } else {
    setMode("ntp");
  }

  setTimezone(tz);
  return true;
}

void TimeService::_refreshTime() {
  if (!_manualMode) {
    if (!getLocalTime(&_timeinfo, 100)) {
      return;
    }
    _wasSynced = true;
    _lastNtpSync = millis();
  }

  _info.hour = _timeinfo.tm_hour;
  _info.minute = _timeinfo.tm_min;
  _info.second = _timeinfo.tm_sec;
  _info.day = _timeinfo.tm_mday;
  _info.month = _timeinfo.tm_mon + 1;
  _info.year = _timeinfo.tm_year + 1900;

  _info.current = _formatTime(_info.hour, _info.minute);
  _info.date = _formatDate(_info.year, _info.month, _info.day);
  strncpy(_info.timezone, _timezone, sizeof(_info.timezone) - 1);
  _info.timezone[sizeof(_info.timezone) - 1] = '\0';
}

String TimeService::_formatDate(int year, int month, int day) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
  return String(buf);
}

String TimeService::_formatTime(int hour, int minute) {
  char buf[8];
  snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
  return String(buf);
}

unsigned long TimeService::getUnixTime() {
  time_t now;
  time(&now);
  return (unsigned long)now;
}

bool TimeService::isNtpSynced() {
  return _ntpActive && _wasSynced;
}

unsigned long TimeService::getTimeSinceSync() {
  if (_lastNtpSync == 0) return 0;
  return millis() - _lastNtpSync;
}

void TimeService::saveTimezone(const char* tz) {
  strncpy(_savedTimezone, tz, sizeof(_savedTimezone) - 1);
  _savedTimezone[sizeof(_savedTimezone) - 1] = '\0';
  configRepo.setTimezone(tz);
  configRepo.save();
  logger.info("TIME", "Timezone saved: %s", tz);
}

String TimeService::loadTimezone() {
  return configRepo.timezone();
}

#endif // SIMULATION
