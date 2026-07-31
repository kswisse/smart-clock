#include "todo_service.h"
#include "../utils/logger.h"
#include "../core/config.h"

TodoService todoService;

void TodoService::begin() {
  todoRepo.begin();
  logger.info("TODO_SVC", "Service initialized (%d todos)", todoRepo.count());
}

std::vector<Todo> TodoService::getAll() {
  return todoRepo.getAll();
}

Todo TodoService::getById(uint16_t id) {
  return todoRepo.getById(id);
}

Todo TodoService::create(const char* title, const char* description, const char* color) {
  Todo todo;
  todo.clear();

  if (!_validateTitle(title)) {
    logger.warn("TODO_SVC", "Invalid title");
    return todo;
  }

  strncpy(todo.title, title, sizeof(todo.title) - 1);
  if (description) strncpy(todo.description, description, sizeof(todo.description) - 1);
  if (color) strncpy(todo.color, color, sizeof(todo.color) - 1);
  todo.completed = false;
  todo.createdAt = millis();

  Todo created = todoRepo.create(todo);
  if (created.id > 0) {
    eventBus.emit(EVT_TODO_CREATED, created.id, created.title);
  }
  return created;
}

bool TodoService::update(uint16_t id, const char* title, const char* description, const char* color) {
  Todo existing = todoRepo.getById(id);
  if (existing.id == 0) {
    logger.warn("TODO_SVC", "Todo %d not found", id);
    return false;
  }

  if (title) strncpy(existing.title, title, sizeof(existing.title) - 1);
  if (description) strncpy(existing.description, description, sizeof(existing.description) - 1);
  if (color) strncpy(existing.color, color, sizeof(existing.color) - 1);

  bool ok = todoRepo.update(id, existing);
  if (ok) {
    eventBus.emit(EVT_TODO_UPDATED, id, existing.title);
  }
  return ok;
}

bool TodoService::toggleComplete(uint16_t id) {
  Todo existing = todoRepo.getById(id);
  if (existing.id == 0) return false;

  existing.completed = !existing.completed;
  bool ok = todoRepo.update(id, existing);
  if (ok) {
    eventBus.emit(EVT_TODO_TOGGLED, id, existing.completed ? "completed" : "pending");
  }
  return ok;
}

bool TodoService::remove(uint16_t id) {
  Todo existing = todoRepo.getById(id);
  if (existing.id == 0) return false;

  bool ok = todoRepo.remove(id);
  if (ok) {
    eventBus.emit(EVT_TODO_DELETED, id, existing.title);
  }
  return ok;
}

size_t TodoService::count() {
  return todoRepo.count();
}

size_t TodoService::pendingCount() {
  size_t count = 0;
  for (const auto& t : todoRepo.getAll()) {
    if (!t.completed) count++;
  }
  return count;
}

std::vector<Todo> TodoService::getPending() {
  std::vector<Todo> result;
  for (const auto& t : todoRepo.getAll()) {
    if (!t.completed) result.push_back(t);
  }
  return result;
}

std::vector<Todo> TodoService::getCompleted() {
  std::vector<Todo> result;
  for (const auto& t : todoRepo.getAll()) {
    if (t.completed) result.push_back(t);
  }
  return result;
}

std::vector<Todo> TodoService::search(const char* query) {
  std::vector<Todo> result;
  if (!query || strlen(query) == 0) return todoRepo.getAll();

  String q = String(query);
  q.toLowerCase();

  for (const auto& t : todoRepo.getAll()) {
    String title = String(t.title);
    title.toLowerCase();
    String desc = String(t.description);
    desc.toLowerCase();

    if (title.indexOf(q) >= 0 || desc.indexOf(q) >= 0) {
      result.push_back(t);
    }
  }
  return result;
}

bool TodoService::_validateTitle(const char* title) {
  if (!title) return false;
  if (strlen(title) == 0) return false;
  if (strlen(title) >= MAX_TITLE_LEN) return false;
  return true;
}
