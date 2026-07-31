#ifdef SIMULATION

#include "../test_common.h"

static int _createdCount = 0;
static int _updatedCount = 0;
static int _deletedCount = 0;
static int _toggledCount = 0;

static void resetTodoCounters() {
  _createdCount = 0;
  _updatedCount = 0;
  _deletedCount = 0;
  _toggledCount = 0;
}

void testTodo() {
  testBeginSuite("3. Todo CRUD");

  // Clean up state from previous phases
  {
    std::vector<Todo> existing = todoService.getAll();
    for (auto& t : existing) todoService.remove(t.id);
    eventBus.processQueue();
  }

  resetTodoCounters();
  eventBus.subscribe(EVT_TODO_CREATED, [](const Event& e) { _createdCount++; });
  eventBus.subscribe(EVT_TODO_UPDATED, [](const Event& e) { _updatedCount++; });
  eventBus.subscribe(EVT_TODO_DELETED, [](const Event& e) { _deletedCount++; });
  eventBus.subscribe(EVT_TODO_TOGGLED, [](const Event& e) { _toggledCount++; });

  // Create
  Todo t1 = todoService.create("Buy milk", "2% fat", "#ff0000");
  testAssert(t1.id > 0, "Todo 1 created with valid id");
  testAssert(strcmp(t1.title, "Buy milk") == 0, "Todo 1 title correct");
  testAssert(strcmp(t1.color, "#ff0000") == 0, "Todo 1 color correct");
  testAssert(t1.completed == false, "Todo 1 not completed by default");
  eventBus.processQueue();
  testAssert(_createdCount == 1, "EVT_TODO_CREATED emitted on create");

  Todo t2 = todoService.create("Buy eggs", " dozen", "#00ff00");
  testAssert(t2.id > t1.id, "Todo 2 has higher id than todo 1");

  // Read
  std::vector<Todo> all = todoService.getAll();
  testAssert(all.size() == 2, "Two todos exist after creating 2");

  Todo fetched = todoService.getById(t1.id);
  testAssert(fetched.id == t1.id, "getById returns correct todo");
  testAssert(strcmp(fetched.title, "Buy milk") == 0, "getById returns correct title");

  Todo notFound = todoService.getById(9999);
  testAssert(notFound.id == 0, "getById returns empty for nonexistent id");

  // Toggle
  testAssert(todoService.toggleComplete(t1.id), "toggleComplete returns true");
  Todo toggled = todoService.getById(t1.id);
  testAssert(toggled.completed == true, "Todo 1 toggled to completed");
  eventBus.processQueue();
  testAssert(_toggledCount == 1, "EVT_TODO_TOGGLED emitted on toggle");

  todoService.toggleComplete(t1.id);
  toggled = todoService.getById(t1.id);
  testAssert(toggled.completed == false, "Todo 1 toggled back to pending");

  // Queries
  todoService.toggleComplete(t2.id);
  std::vector<Todo> completed = todoService.getCompleted();
  testAssert(completed.size() == 1, "getCompleted returns 1");
  testAssert(completed[0].id == t2.id, "getCompleted returns correct todo");

  std::vector<Todo> pending = todoService.getPending();
  testAssert(pending.size() == 1, "getPending returns 1");
  testAssert(pending[0].id == t1.id, "getPending returns correct todo");

  testAssert(todoService.count() == 2, "count() returns 2");
  testAssert(todoService.pendingCount() == 1, "pendingCount() returns 1");

  // Search
  std::vector<Todo> found = todoService.search("milk");
  testAssert(found.size() == 1, "search('milk') finds 1 result");
  testAssert(found[0].id == t1.id, "search('milk') finds correct todo");

  // Delete
  testAssert(todoService.remove(t2.id), "remove returns true");
  all = todoService.getAll();
  testAssert(all.size() == 1, "One todo remaining after delete");
  eventBus.processQueue();
  testAssert(_deletedCount == 1, "EVT_TODO_DELETED emitted on delete");

  testAssert(!todoService.remove(9999), "remove nonexistent returns false");

  // Validation
  Todo bad = todoService.create("", "empty title", "#fff");
  testAssert(bad.id == 0, "Create with empty title rejected");

  Todo nullTitle = todoService.create(nullptr, "null title", "#fff");
  testAssert(nullTitle.id == 0, "Create with null title rejected");

  // Persistence: create, then verify via repo reload
  Todo t3 = todoService.create("Persist me", "check", "#123");
  testAssert(t3.id > 0, "Todo created for persistence test");
  todoRepo.save();
  todoRepo.load();
  std::vector<Todo> reloaded = todoRepo.getAll();
  bool foundPersist = false;
  for (auto& t : reloaded) {
    if (t.id == t3.id && strcmp(t.title, "Persist me") == 0) foundPersist = true;
  }
  testAssert(foundPersist, "Todo survives save/load cycle");

  // Clean up
  eventBus.unsubscribe(EVT_TODO_CREATED);
  eventBus.unsubscribe(EVT_TODO_UPDATED);
  eventBus.unsubscribe(EVT_TODO_DELETED);
  eventBus.unsubscribe(EVT_TODO_TOGGLED);
  todoService.remove(t1.id);
  todoService.remove(t3.id);
  testEndSuite();
}

#endif
