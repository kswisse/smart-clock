#ifndef TFT_TODO_H
#define TFT_TODO_H

#include <Arduino.h>
#include "../models/models.h"
#include <vector>

#ifndef SIMULATION
class TftTodo {
public:
  void init();
  void render(const std::vector<Todo>& todos, int pendingCount);
  void renderItem(const Todo& todo, int y);
  void renderHeader(int pendingCount);
  void clear();

private:
  bool _initialized;
  int _scrollOffset;
  int _selectedIndex;
};

extern TftTodo tftTodo;

#endif // SIMULATION
#endif // TFT_TODO_H
