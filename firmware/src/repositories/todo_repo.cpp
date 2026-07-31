#include "todo_repo.h"
#include "repository.h"
#include "../core/config.h"

TodoRepository todoRepo;

bool TodoRepository::begin() {
  _mutex = xSemaphoreCreateMutex();
  if (_mutex == NULL) {
    logger.error("TODO_REPO", "Mutex creation failed");
    return false;
  }
  _nextId = 1;
  return load();
}

bool TodoRepository::load() {
  _lock();
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  if (!repository.readJson(TODO_FILE, doc)) {
    logger.warn("TODO_REPO", "No todo file, starting empty");
    _todos.clear();
    _unlock();
    return true;
  }

  _todos.clear();
  _nextId = doc["nextId"] | 1;
  JsonArray arr = doc["todos"].as<JsonArray>();

  for (JsonObject obj : arr) {
    Todo todo;
    todo.clear();
    todo.id = obj["id"];
    strncpy(todo.title, obj["title"] | "", sizeof(todo.title) - 1);
    strncpy(todo.description, obj["description"] | "", sizeof(todo.description) - 1);
    strncpy(todo.color, obj["color"] | "#4fc3f7", sizeof(todo.color) - 1);
    todo.completed = obj["completed"] | false;
    todo.createdAt = obj["created_at"] | 0UL;
    _todos.push_back(todo);
  }

  logger.info("TODO_REPO", "Loaded %d todos", _todos.size());
  _unlock();
  return true;
}

bool TodoRepository::save() {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  doc["nextId"] = _nextId;
  JsonArray arr = doc.createNestedArray("todos");

  for (const auto& todo : _todos) {
    JsonObject obj = arr.createNestedObject();
    obj["id"] = todo.id;
    obj["title"] = todo.title;
    obj["description"] = todo.description;
    obj["color"] = todo.color;
    obj["completed"] = todo.completed;
    obj["created_at"] = todo.createdAt;
  }

  bool ok = repository.writeJson(TODO_FILE, doc);
  if (ok) {
    logger.debug("TODO_REPO", "Saved %d todos", _todos.size());
  }
  return ok;
}

std::vector<Todo> TodoRepository::getAll() {
  _lock();
  std::vector<Todo> copy = _todos;
  _unlock();
  return copy;
}

Todo TodoRepository::getById(uint16_t id) {
  _lock();
  for (const auto& todo : _todos) {
    if (todo.id == id) {
      Todo result = todo;
      _unlock();
      return result;
    }
  }
  _unlock();
  Todo empty;
  empty.clear();
  return empty;
}

Todo TodoRepository::create(const Todo& todo) {
  _lock();
  if (_todos.size() >= TODO_MAX) {
    logger.warn("TODO_REPO", "Todo limit reached (%d)", TODO_MAX);
    _unlock();
    Todo empty;
    empty.clear();
    return empty;
  }

  Todo newTodo = todo;
  newTodo.id = _generateId();
  if (newTodo.createdAt == 0) newTodo.createdAt = millis();
  _todos.push_back(newTodo);
  save();
  _unlock();

  logger.info("TODO_REPO", "Created todo %d: %s", newTodo.id, newTodo.title);
  return newTodo;
}

bool TodoRepository::update(uint16_t id, const Todo& todo) {
  _lock();
  for (size_t i = 0; i < _todos.size(); i++) {
    if (_todos[i].id == id) {
      _todos[i] = todo;
      _todos[i].id = id;
      save();
      _unlock();
      logger.info("TODO_REPO", "Updated todo %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

bool TodoRepository::remove(uint16_t id) {
  _lock();
  for (size_t i = 0; i < _todos.size(); i++) {
    if (_todos[i].id == id) {
      _todos.erase(_todos.begin() + i);
      save();
      _unlock();
      logger.info("TODO_REPO", "Deleted todo %d", id);
      return true;
    }
  }
  _unlock();
  return false;
}

size_t TodoRepository::count() {
  _lock();
  size_t c = _todos.size();
  _unlock();
  return c;
}

uint16_t TodoRepository::_generateId() {
  return _nextId++;
}

void TodoRepository::_lock() {
  xSemaphoreTake(_mutex, pdMS_TO_TICKS(REPO_LOCK_TIMEOUT_MS));
}

void TodoRepository::_unlock() {
  xSemaphoreGive(_mutex);
}
