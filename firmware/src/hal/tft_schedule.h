#ifndef TFT_SCHEDULE_H
#define TFT_SCHEDULE_H

#include <Arduino.h>
#include <vector>
#include "../models/models.h"

#ifndef SIMULATION
class TftSchedule {
public:
  void init();
  void renderToday(const std::vector<ScheduleEntry>& entries);
  void renderWeek(const std::vector<ScheduleEntry>& entries);
  void setSelectedIndex(int index);
  void invalidate();

private:
  bool _initialized;
  bool _dirty;
  int _selectedIndex;

  void _renderHeader(int count);
  void _renderEntry(const ScheduleEntry& entry, int y, bool selected);
  void _renderEmpty();
};

extern TftSchedule tftSchedule;

#endif // SIMULATION
#endif // TFT_SCHEDULE_H
