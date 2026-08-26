#ifndef SCHEDULE_HANDLERS_H
#define SCHEDULE_HANDLERS_H

#include <Arduino.h>
#include <AsyncWebServer.h>
#include "../models/models.h"

class ScheduleHandlers {
public:
  void handleGetAll(AsyncWebServerRequest* request);
  void handleGetById(AsyncWebServerRequest* request, uint16_t id);
  void handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
  void handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total);
  void handleDelete(AsyncWebServerRequest* request, uint16_t id);

private:
  String _entryToJson(const ScheduleEntry& entry);
  String _entriesToJson(const std::vector<ScheduleEntry>& entries);
};

extern ScheduleHandlers scheduleHandlers;

#endif // SCHEDULE_HANDLERS_H
