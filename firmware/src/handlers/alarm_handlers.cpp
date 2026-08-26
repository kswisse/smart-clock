#include "alarm_handlers.h"

#ifndef SIMULATION
#include "api_response.h"
#include "../models/models.h"
#include "../services/alarm_service.h"
#include "../services/wifi_service.h"
#include "../core/config.h"
#include <ArduinoJson.h>

AlarmHandlers alarmHandlers;

void AlarmHandlers::handleGetAll(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  auto alarms = alarmService.getAll();
  String json = _alarmsToJson(alarms);
  ApiResponse::ok(request, "Alarms retrieved", json.c_str());
}

void AlarmHandlers::handleGetById(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  Alarm alarm = alarmService.getById(id);
  if (alarm.id == 0) {
    ApiResponse::notFound(request, "Alarm not found");
    return;
  }
  String json = _alarmToJson(alarm);
  ApiResponse::ok(request, "Alarm retrieved", json.c_str());
}

void AlarmHandlers::handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return;

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  uint8_t hour = doc["hour"] | 0;
  uint8_t minute = doc["minute"] | 0;
  const char* sound = doc["sound"] | "default";
  uint8_t volume = doc["volume"] | 50;

  uint8_t repeatDays[7];
  uint8_t repeatCount = 0;
  JsonArray days = doc["repeat"].as<JsonArray>();
  for (int i = 0; i < days.size() && i < 7; i++) {
    repeatDays[i] = days[i].as<int>();
    repeatCount++;
  }

  Alarm created = alarmService.create(hour, minute, repeatDays, repeatCount, sound, volume);
  if (created.id == 0) {
    ApiResponse::badRequest(request, "Failed to create alarm");
    return;
  }

  String json = _alarmToJson(created);
  ApiResponse::created(request, "Alarm created", json.c_str());
}

void AlarmHandlers::handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return;

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  // Handle enabled toggle
  if (doc.containsKey("enabled")) {
    bool enabled = doc["enabled"];
    alarmService.update(id, enabled);
  }

  // Handle full update
  if (doc.containsKey("hour") || doc.containsKey("minute")) {
    Alarm existing = alarmService.getById(id);
    if (existing.id == 0) {
      ApiResponse::notFound(request, "Alarm not found");
      return;
    }

    if (doc.containsKey("hour")) existing.hour = doc["hour"];
    if (doc.containsKey("minute")) existing.minute = doc["minute"];
    if (doc.containsKey("sound")) strncpy(existing.sound, doc["sound"] | "default", sizeof(existing.sound) - 1);
    if (doc.containsKey("volume")) existing.volume = doc["volume"];
    if (doc.containsKey("repeat")) {
      JsonArray days = doc["repeat"].as<JsonArray>();
      for (int i = 0; i < 7; i++) existing.repeatDays[i] = false;
      for (int i = 0; i < days.size() && i < 7; i++) {
        int day = days[i];
        if (day >= 0 && day < 7) existing.repeatDays[day] = true;
      }
    }

    alarmService.updateAlarm(id, existing);
  }

  Alarm updated = alarmService.getById(id);
  String json = _alarmToJson(updated);
  ApiResponse::ok(request, "Alarm updated", json.c_str());
}

void AlarmHandlers::handleDelete(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  bool ok = alarmService.remove(id);
  if (!ok) {
    ApiResponse::notFound(request, "Alarm not found");
    return;
  }
  ApiResponse::ok(request, "Alarm deleted");
}

String AlarmHandlers::_alarmToJson(const Alarm& alarm) {
  StaticJsonDocument<512> doc;
  doc["id"] = alarm.id;
  doc["hour"] = alarm.hour;
  doc["minute"] = alarm.minute;
  doc["enabled"] = alarm.enabled;
  doc["sound"] = alarm.sound;
  doc["volume"] = alarm.volume;
  JsonArray days = doc.createNestedArray("repeat");
  for (int i = 0; i < 7; i++) {
    if (alarm.repeatDays[i]) days.add(i);
  }

  String output;
  serializeJson(doc, output);
  return output;
}

String AlarmHandlers::_alarmsToJson(const std::vector<Alarm>& alarms) {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  JsonArray arr = doc.to<JsonArray>();

  for (const auto& alarm : alarms) {
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

  String output;
  serializeJson(doc, output);
  return output;
}

#endif // SIMULATION
