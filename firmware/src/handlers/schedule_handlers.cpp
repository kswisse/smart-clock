#include "schedule_handlers.h"

#ifndef SIMULATION
#include "api_response.h"
#include "request_body.h"
#include "../models/models.h"
#include "../services/schedule_service.h"
#include "../services/wifi_service.h"
#include "../core/config.h"
#include <ArduinoJson.h>

ScheduleHandlers scheduleHandlers;

void ScheduleHandlers::handleGetAll(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  // Optional: ?day=0-6
  uint8_t day = 255;
  if (request->hasParam("day")) {
    day = request->getParam("day")->value().toInt();
  }

  if (day <= 6) {
    auto entries = scheduleService.getByDay(day);
    String json = _entriesToJson(entries);
    ApiResponse::ok(request, "Schedule retrieved", json.c_str());
  } else {
    auto entries = scheduleService.getAll();
    String json = _entriesToJson(entries);
    ApiResponse::ok(request, "Schedule retrieved", json.c_str());
  }
}

void ScheduleHandlers::handleGetById(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  ScheduleEntry entry = scheduleService.getById(id);
  if (entry.id == 0) {
    ApiResponse::notFound(request, "Entry not found");
    return;
  }
  String json = _entryToJson(entry);
  ApiResponse::ok(request, "Entry retrieved", json.c_str());
}

void ScheduleHandlers::handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  const char* body = collect_request_body(request, data, len, index, total, 512);
  if (!body) return;

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  uint8_t day = doc["day"] | 0;
  const char* start = doc["start"] | "00:00";
  const char* end = doc["end"] | "00:00";
  const char* title = doc["title"] | "";
  const char* color = doc["color"] | "#4fc3f7";

  if (day > 6) {
    ApiResponse::badRequest(request, "Invalid day (0-6)");
    return;
  }

  ScheduleEntry created = scheduleService.create(day, start, end, title, color);
  if (created.id == 0) {
    ApiResponse::badRequest(request, "Failed to create entry");
    return;
  }

  String json = _entryToJson(created);
  ApiResponse::created(request, "Entry created", json.c_str());
}

void ScheduleHandlers::handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  const char* body = collect_request_body(request, data, len, index, total, 512);
  if (!body) return;

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  ScheduleEntry existing = scheduleService.getById(id);
  if (existing.id == 0) {
    ApiResponse::notFound(request, "Entry not found");
    return;
  }

  if (doc.containsKey("day")) existing.day = doc["day"];
  if (doc.containsKey("start")) strncpy(existing.startTime, doc["start"] | "00:00", sizeof(existing.startTime) - 1);
  if (doc.containsKey("end")) strncpy(existing.endTime, doc["end"] | "00:00", sizeof(existing.endTime) - 1);
  if (doc.containsKey("title")) strncpy(existing.title, doc["title"] | "", sizeof(existing.title) - 1);
  if (doc.containsKey("color")) strncpy(existing.color, doc["color"] | "#4fc3f7", sizeof(existing.color) - 1);

  scheduleService.update(id, existing);

  String json = _entryToJson(existing);
  ApiResponse::ok(request, "Entry updated", json.c_str());
}

void ScheduleHandlers::handleDelete(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  bool ok = scheduleService.remove(id);
  if (!ok) {
    ApiResponse::notFound(request, "Entry not found");
    return;
  }
  ApiResponse::ok(request, "Entry deleted");
}

String ScheduleHandlers::_entryToJson(const ScheduleEntry& entry) {
  StaticJsonDocument<256> doc;
  doc["id"] = entry.id;
  doc["day"] = entry.day;
  doc["start"] = entry.startTime;
  doc["end"] = entry.endTime;
  doc["title"] = entry.title;
  doc["color"] = entry.color;

  String output;
  serializeJson(doc, output);
  return output;
}

String ScheduleHandlers::_entriesToJson(const std::vector<ScheduleEntry>& entries) {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  JsonArray arr = doc.to<JsonArray>();

  for (const auto& entry : entries) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = entry.id;
    obj["day"] = entry.day;
    obj["start"] = entry.startTime;
    obj["end"] = entry.endTime;
    obj["title"] = entry.title;
    obj["color"] = entry.color;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

#endif // SIMULATION
