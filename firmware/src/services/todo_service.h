#ifndef TODO_SERVICE_H
#define TODO_SERVICE_H

#include <Arduino.h>
#include <vector>
#include "../models/models.h"
#include "../repositories/todo_repo.h"
#include "../events/event_bus.h"

class TodoService {
public:
  void begin();

  // CRUD via service layer
  std::vector<Todo> getAll();
  Todo getById(uint16_t id);
  Todo create(const char* title, const char* description, const char* color);
  bool update(uint16_t id, const char* title, const char* description, const char* color);
  bool toggleComplete(uint16_t id);
  bool remove(uint16_t id);

  // Queries
  size_t count();
  size_t pendingCount();
  std::vector<Todo> getPending();
  std::vector<Todo> getCompleted();
  std::vector<Todo> search(const char* query);

private:
  bool _validateTitle(const char* title);
};

extern TodoService todoService;

#endif // TODO_SERVICE_H
