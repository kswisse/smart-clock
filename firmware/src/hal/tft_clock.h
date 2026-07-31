#ifndef TFT_CLOCK_H
#define TFT_CLOCK_H

#include <Arduino.h>

#ifndef SIMULATION
class TftClock {
public:
  void init();
  void render(uint8_t hour, uint8_t minute, uint8_t second);
  void renderDate(uint16_t year, uint8_t month, uint8_t day, uint8_t dayOfWeek);
  void invalidate();

private:
  bool _initialized;
  bool _dirty;
  uint8_t _lastSecond;

  void _drawTime(uint8_t hour, uint8_t minute, uint8_t second);
  void _drawColon(int x, int y, bool visible);
};

extern TftClock tftClock;

#endif // SIMULATION
#endif // TFT_CLOCK_H
