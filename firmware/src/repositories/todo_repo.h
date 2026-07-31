#ifndef TODO_REPOSITORY_H
#define TODO_REPOSITORY_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <mutex>
#include "../models/models.h"
#include "../utils/logger.h"

#define TODO_FILE "/todos.json"
#define TODO_MAX 50
#define REPO_LOCK_TIMEOUT_MS 100

class TodoRepository {
public:
  bool begin();
  bool load();
  bool save();

  // CRUD
  std::vector<Todo> getAll();
  Todo getById(uint16_t id);
  Todo create(const Todo& todo);
  bool update(uint16_t id, const Todo& todo);
  bool remove(uint16_t id);
  size_t count();

private:
  std::vector<Todo> _todos;
  uint16_t _nextId;
  SemaphoreHandle_t _mutex;

  uint16_t _generateId();
  void _lock();
  void _unlock();
};

extern TodoRepository todoRepo;

#endif // TODO_REPOSITORY_H
