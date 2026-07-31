#include "tft_todo.h"

#ifndef SIMULATION
#include "../utils/logger.h"

TftTodo tftTodo;

void TftTodo::init() {
  _initialized = true;
  _scrollOffset = 0;
  _selectedIndex = -1;
  logger.info("TFT_TODO", "TFT Todo renderer initialized (stub)");
}

void TftTodo::render(const std::vector<Todo>& todos, int pendingCount) {
  if (!_initialized) return;

  // TODO: Implement actual TFT rendering
  // Example layout:
  // ┌─────────────────────┐
  // │ Todos (3 pending)   │
  // ├─────────────────────┤
  // │ ☐ Buy groceries     │
  // │ ☑ Fix bug #123      │
  // │ ☐ Call dentist      │
  // └─────────────────────┘

  logger.debug("TFT_TODO", "Rendering %d todos (%d pending)", todos.size(), pendingCount);
}

void TftTodo::renderItem(const Todo& todo, int y) {
  // TODO: Render single todo item at y position
  // Draw checkbox, title, color indicator
}

void TftTodo::renderHeader(int pendingCount) {
  // TODO: Render header with pending count
}

void TftTodo::clear() {
  // TODO: Clear todo area on display
}

#endif // SIMULATION
