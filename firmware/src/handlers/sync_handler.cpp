#include "sync_handler.h"
#include "api_response.h"
#include "request_body.h"
#include "../core/config.h"
#include "../models/models.h"
#include "../repositories/todo_repo.h"
#include "../repositories/alarm_repo.h"
#include "../services/time_service.h"
#include "../services/wifi_service.h"
#include "../utils/logger.h"
#include <ArduinoJson.h>

SyncHandler syncHandler;

void SyncHandler::handleSyncPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  const char* body = collect_request_body(request, data, len, index, total, JSON_DOC_LARGE);
  if (!body) return;

  DynamicJsonDocument doc(JSON_DOC_LARGE);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  // Backup current state
  std::vector<Todo> oldTodos = todoRepo.getAll();
  std::vector<Alarm> oldAlarms = alarmRepo.getAll();
  TimeInfo oldTime = timeService.getTime();
  bool backupTodosOk = false;
  bool backupAlarmsOk = false;
  bool backupTimeOk = false;
  bool success = false;

  // 1. Set time
  if (doc.containsKey("time")) {
    JsonObject timeObj = doc["time"];
    int year = timeObj["year"] | 2026;
    int month = timeObj["month"] | 1;
    int day = timeObj["day"] | 1;
    int hour = timeObj["hour"] | 0;
    int minute = timeObj["minute"] | 0;
    int second = timeObj["second"] | 0;
    const char* tz = timeObj["timezone"] | "Asia/Ho_Chi_Minh";
    timeService.setManual(year, month, day, hour, minute, second);
    timeService.saveTimezone(tz);
    backupTimeOk = true;
  }

  // 2. Replace todos
  if (doc.containsKey("todos")) {
    JsonArray todosArr = doc["todos"].as<JsonArray>();
    if (todosArr.size() > TODO_MAX) {
      ApiResponse::badRequest(request, "Too many todos (max 50)");
      goto restore;
    }

    for (const auto& t : oldTodos) todoRepo.remove(t.id);
    backupTodosOk = true;

    for (JsonObject todoObj : todosArr) {
      Todo todo;
      todo.clear();
      todo.id = todoObj["id"] | 0;
      strncpy(todo.title, todoObj["title"] | "", sizeof(todo.title) - 1);
      strncpy(todo.description, todoObj["description"] | "", sizeof(todo.description) - 1);
      strncpy(todo.color, todoObj["color"] | "#4fc3f7", sizeof(todo.color) - 1);
      todo.completed = todoObj["completed"] | false;
      todo.createdAt = todoObj["created_at"] | 0UL;

      Todo created = todoRepo.create(todo);
      if (created.id == 0) {
        ApiResponse::serverError(request, "Failed to create todo");
        goto restore;
      }
    }
  }

  // 3. Replace alarms
  if (doc.containsKey("alarms")) {
    JsonArray alarmsArr = doc["alarms"].as<JsonArray>();
    if (alarmsArr.size() > ALARM_MAX) {
      ApiResponse::badRequest(request, "Too many alarms (max 10)");
      goto restore;
    }

    for (const auto& a : oldAlarms) alarmRepo.remove(a.id);
    backupAlarmsOk = true;

    for (JsonObject alarmObj : alarmsArr) {
      Alarm alarm;
      alarm.clear();
      alarm.hour = alarmObj["hour"] | 0;
      alarm.minute = alarmObj["minute"] | 0;
      alarm.enabled = alarmObj["enabled"] | true;
      strncpy(alarm.sound, alarmObj["sound"] | "default", sizeof(alarm.sound) - 1);
      alarm.volume = alarmObj["volume"] | 50;

      JsonArray days = alarmObj["repeat"].as<JsonArray>();
      for (int i = 0; i < 7; i++) alarm.repeatDays[i] = false;
      for (int i = 0; i < days.size() && i < 7; i++) {
        int day = days[i];
        if (day >= 0 && day < 7) alarm.repeatDays[day] = true;
      }

      Alarm created = alarmRepo.create(alarm);
      if (created.id == 0) {
        ApiResponse::serverError(request, "Failed to create alarm");
        goto restore;
      }
    }
  }

  success = true;
  {
    StaticJsonDocument<256> resp;
    resp["todos_synced"] = doc.containsKey("todos") ? doc["todos"].size() : 0;
    resp["alarms_synced"] = doc.containsKey("alarms") ? doc["alarms"].size() : 0;
    ApiResponse::ok(request, "Sync successful", resp);
  }
  return;

restore:
  // Rollback on failure
  if (backupTodosOk) {
    for (const auto& t : todoRepo.getAll()) todoRepo.remove(t.id);
    for (const auto& t : oldTodos) todoRepo.create(t);
  }
  if (backupAlarmsOk) {
    for (const auto& a : alarmRepo.getAll()) alarmRepo.remove(a.id);
    for (const auto& a : oldAlarms) alarmRepo.create(a);
  }
  if (backupTimeOk && !success) {
    timeService.setManual(oldTime.year, oldTime.month, oldTime.day,
                          oldTime.hour, oldTime.minute, oldTime.second);
    timeService.setTimezone(oldTime.timezone);
    timeService.saveTimezone(oldTime.timezone);
  }
}
