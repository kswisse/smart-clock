#ifndef ALARM_HANDLERS_H
#define ALARM_HANDLERS_H

#include <Arduino.h>
#include <AsyncWebServer.h>

class AlarmHandlers {
public:
  void handleGetAll(AsyncWebServerRequest* request);
  void handleGetById(AsyncWebServerRequest* request, uint16_t id);
  void handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
  void handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total);
  void handleDelete(AsyncWebServerRequest* request, uint16_t id);

private:
  String _alarmToJson(const Alarm& alarm);
  String _alarmsToJson(const std::vector<Alarm>& alarms);
};

extern AlarmHandlers alarmHandlers;

#endif // ALARM_HANDLERS_H
