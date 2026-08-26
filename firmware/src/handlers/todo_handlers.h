#ifndef TODO_HANDLERS_H
#define TODO_HANDLERS_H

#include <Arduino.h>
#include <AsyncWebServer.h>
#include "../models/models.h"

class TodoHandlers {
public:
  void handleGetAll(AsyncWebServerRequest* request);
  void handleGetById(AsyncWebServerRequest* request, uint16_t id);
  void handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
  void handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total);
  void handleDelete(AsyncWebServerRequest* request, uint16_t id);

private:
  String _todoToJson(const Todo& todo);
  String _todosToJson(const std::vector<Todo>& todos);
};

extern TodoHandlers todoHandlers;

#endif // TODO_HANDLERS_H
