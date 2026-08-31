#include "todo_handlers.h"

#ifndef SIMULATION
#include "api_response.h"
#include "request_body.h"
#include "../models/models.h"
#include "../services/todo_service.h"
#include "../services/wifi_service.h"
#include "../core/config.h"
#include <ArduinoJson.h>

TodoHandlers todoHandlers;

void TodoHandlers::handleGetAll(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  auto todos = todoService.getAll();
  String json = _todosToJson(todos);
  ApiResponse::ok(request, "Todos retrieved", json.c_str());
}

void TodoHandlers::handleGetById(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  Todo todo = todoService.getById(id);
  if (todo.id == 0) {
    ApiResponse::notFound(request, "Todo not found");
    return;
  }
  String json = _todoToJson(todo);
  ApiResponse::ok(request, "Todo retrieved", json.c_str());
}

void TodoHandlers::handleCreate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  const char* body = collect_request_body(request, data, len, index, total, 512);
  if (!body) return;

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  const char* title = doc["title"] | "";
  const char* description = doc["description"] | "";
  const char* color = doc["color"] | "#4fc3f7";

  Todo created = todoService.create(title, description, color);
  if (created.id == 0) {
    ApiResponse::badRequest(request, "Failed to create todo");
    return;
  }

  String json = _todoToJson(created);
  ApiResponse::created(request, "Todo created", json.c_str());
}

void TodoHandlers::handleUpdate(AsyncWebServerRequest* request, uint16_t id, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  const char* body = collect_request_body(request, data, len, index, total, 512);
  if (!body) return;

  StaticJsonDocument<512> doc;
  if (!deserializeJson(doc, body)) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  const char* title = doc.containsKey("title") ? doc["title"].as<const char*>() : nullptr;
  const char* description = doc.containsKey("description") ? doc["description"].as<const char*>() : nullptr;
  const char* color = doc.containsKey("color") ? doc["color"].as<const char*>() : nullptr;

  // Handle completed toggle separately
  if (doc.containsKey("completed")) {
    bool completed = doc["completed"];
    Todo existing = todoService.getById(id);
    if (existing.id == 0) {
      ApiResponse::notFound(request, "Todo not found");
      return;
    }
    if (existing.completed != completed) {
      todoService.toggleComplete(id);
    }
  }

  if (title || description || color) {
    bool ok = todoService.update(id, title, description, color);
    if (!ok) {
      ApiResponse::notFound(request, "Todo not found");
      return;
    }
  }

  Todo updated = todoService.getById(id);
  String json = _todoToJson(updated);
  ApiResponse::ok(request, "Todo updated", json.c_str());
}

void TodoHandlers::handleDelete(AsyncWebServerRequest* request, uint16_t id) {
  wifiService.touchActivity();
  bool ok = todoService.remove(id);
  if (!ok) {
    ApiResponse::notFound(request, "Todo not found");
    return;
  }
  ApiResponse::ok(request, "Todo deleted");
}

String TodoHandlers::_todoToJson(const Todo& todo) {
  StaticJsonDocument<512> doc;
  doc["id"] = todo.id;
  doc["title"] = todo.title;
  doc["description"] = todo.description;
  doc["color"] = todo.color;
  doc["completed"] = todo.completed;
  doc["created_at"] = todo.createdAt;

  String output;
  serializeJson(doc, output);
  return output;
}

String TodoHandlers::_todosToJson(const std::vector<Todo>& todos) {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  JsonArray arr = doc.to<JsonArray>();

  for (const auto& todo : todos) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = todo.id;
    obj["title"] = todo.title;
    obj["description"] = todo.description;
    obj["color"] = todo.color;
    obj["completed"] = todo.completed;
    obj["created_at"] = todo.createdAt;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

#endif // SIMULATION
