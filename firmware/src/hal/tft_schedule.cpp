#include "tft_schedule.h"

#ifndef SIMULATION
#include "display_hal.h"
#include "../core/config.h"
#include "../utils/logger.h"

#define SCH_ITEM_HEIGHT 28

TftSchedule tftSchedule;

void TftSchedule::init() {
  _initialized = true;
  _dirty = true;
  _selectedIndex = -1;
  logger.info("TFT_SCH", "Schedule renderer init");
}

void TftSchedule::renderToday(const std::vector<ScheduleEntry>& entries) {
  if (!_initialized || !_dirty) return;

  displayHAL.fillRect(0, CONTENT_Y, DISPLAY_WIDTH, CONTENT_H, 0x0000);
  _renderHeader(entries.size());

  if (entries.empty()) {
    _renderEmpty();
    _dirty = false;
    return;
  }

  int startY = CONTENT_Y + 24;
  int maxVisible = min(CONTENT_H / SCH_ITEM_HEIGHT, (int)entries.size());

  for (int i = 0; i < maxVisible; i++) {
    bool selected = (i == _selectedIndex);
    _renderEntry(entries[i], startY + i * SCH_ITEM_HEIGHT, selected);
  }

  _dirty = false;
}

void TftSchedule::renderWeek(const std::vector<ScheduleEntry>& entries) {
  if (!_initialized || !_dirty) return;

  displayHAL.fillRect(0, CONTENT_Y, DISPLAY_WIDTH, CONTENT_H, 0x0000);

  const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  int colW = DISPLAY_WIDTH / 7;

  // Day headers
  for (int d = 0; d < 7; d++) {
    uint16_t color = (d == 0 || d == 6) ? 0xF800 : 0xFFFF;  // Red for weekends
    displayHAL.printAt(d * colW + 4, CONTENT_Y + 4, days[d], 1, color, 0x0000);
  }

  displayHAL.drawLine(0, CONTENT_Y + 20, DISPLAY_WIDTH, CONTENT_Y + 20, 0x4208);

  // Count entries per day
  int counts[7] = {0};
  for (const auto& e : entries) {
    if (e.day < 7) counts[e.day]++;
  }

  // Show counts
  for (int d = 0; d < 7; d++) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", counts[d]);
    displayHAL.printAt(d * colW + 12, CONTENT_Y + 40, buf, 2, 0x07FF, 0x0000);
  }

  _dirty = false;
}

void TftSchedule::setSelectedIndex(int index) {
  _selectedIndex = index;
  _dirty = true;
}

void TftSchedule::invalidate() {
  _dirty = true;
}

void TftSchedule::_renderHeader(int count) {
  char header[32];
  snprintf(header, sizeof(header), "Schedule (%d)", count);
  displayHAL.fillRect(0, CONTENT_Y, DISPLAY_WIDTH, 24, 0x2104);
  displayHAL.printAt(8, CONTENT_Y + 4, header, 1, 0xFFFF, 0x2104);
  displayHAL.drawLine(0, CONTENT_Y + 23, DISPLAY_WIDTH, CONTENT_Y + 23, 0x4208);
}

void TftSchedule::_renderEntry(const ScheduleEntry& entry, int y, bool selected) {
  uint16_t bgColor = selected ? 0x1082 : 0x0000;

  // Time range
  char timeRange[16];
  snprintf(timeRange, sizeof(timeRange), "%s-%s", entry.startTime, entry.endTime);
  displayHAL.printAt(8, y + 4, timeRange, 1, 0xFFFF, bgColor);

  // Color bar (parse hex color)
  displayHAL.fillRect(DISPLAY_WIDTH - 60, y + 4, 4, SCH_ITEM_HEIGHT - 8, 0x07FF);

  // Title
  displayHAL.printAt(80, y + 4, entry.title, 1, 0xBDF7, bgColor);

  // Separator
  displayHAL.drawLine(8, y + SCH_ITEM_HEIGHT - 1, DISPLAY_WIDTH - 8, y + SCH_ITEM_HEIGHT - 1, 0x2104);
}

void TftSchedule::_renderEmpty() {
  displayHAL.printAt(DISPLAY_WIDTH / 2 - 40, CONTENT_Y + 60, "No events", 1, 0x8410, 0x0000);
}

#endif // SIMULATION
