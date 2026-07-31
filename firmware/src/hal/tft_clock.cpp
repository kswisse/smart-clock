#include "tft_clock.h"

#ifndef SIMULATION
#include "display_hal.h"
#include "../core/config.h"
#include "../utils/logger.h"

static const char* DAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* MONTH_NAMES[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

TftClock tftClock;

void TftClock::init() {
  _initialized = true;
  _dirty = true;
  _lastSecond = 255;
  logger.info("TFT_CLK", "Clock renderer init");
}

void TftClock::render(uint8_t hour, uint8_t minute, uint8_t second) {
  if (!_initialized) return;
  if (!_dirty && second == _lastSecond) return;

  _drawTime(hour, minute, second);
  _lastSecond = second;
  _dirty = false;
}

void TftClock::renderDate(uint16_t year, uint8_t month, uint8_t day, uint8_t dayOfWeek) {
  if (!_initialized) return;

  char dateStr[32];
  snprintf(dateStr, sizeof(dateStr), "%s %s %d %d",
           DAY_NAMES[dayOfWeek], MONTH_NAMES[month], day, year);

  displayHAL.printAt(4, CLOCK_AREA_Y + CLOCK_AREA_H - 20, dateStr, 1, 0x8410, 0x0000);
}

void TftClock::invalidate() {
  _dirty = true;
}

void TftClock::_drawTime(uint8_t hour, uint8_t minute, uint8_t second) {
  char timeBuf[6];
  snprintf(timeBuf, sizeof(timeBuf), "%02d%02d", hour, minute);

  // Clear clock area
  displayHAL.fillRect(0, CLOCK_AREA_Y, DISPLAY_WIDTH, CLOCK_AREA_H, 0x0000);

  // Big time digits
  int centerX = DISPLAY_WIDTH / 2;
  int digitWidth = 36;  // Approx width for size 4 font
  int startX = centerX - (digitWidth * 4 + 16) / 2;  // 4 digits + colon space
  int y = CLOCK_AREA_Y + 20;

  // Hour tens
  displayHAL.printAt(startX, y, String(timeBuf[0]).c_str(), 4, 0xFFFF, 0x0000);
  // Hour units
  displayHAL.printAt(startX + digitWidth, y, String(timeBuf[1]).c_str(), 4, 0xFFFF, 0x0000);

  // Colon (blinking on even seconds)
  _drawColon(startX + digitWidth * 2 + 4, y + 10, second % 2 == 0);

  // Minute tens
  displayHAL.printAt(startX + digitWidth * 2 + 20, y, String(timeBuf[2]).c_str(), 4, 0x07FF, 0x0000);
  // Minute units
  displayHAL.printAt(startX + digitWidth * 3 + 20, y, String(timeBuf[3]).c_str(), 4, 0x07FF, 0x0000);
}

void TftClock::_drawColon(int x, int y, bool visible) {
  uint16_t color = visible ? 0xFFFF : 0x0000;
  displayHAL.fillCircle(x, y, 3, color);
  displayHAL.fillCircle(x, y + 20, 3, color);
}

#endif // SIMULATION
