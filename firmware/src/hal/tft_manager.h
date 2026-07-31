#ifndef TFT_MANAGER_H
#define TFT_MANAGER_H

#include <Arduino.h>
#include "../navigation/nav_state.h"

#ifndef SIMULATION
class TftManager {
public:
  void init();
  void update();
  void renderCurrentScreen();
  void forceRedraw();
  void onScreenChanged(ScreenType from, ScreenType to);

  // Frame metrics
  unsigned long getLastFrameTime();
  float getAverageFrameTime();
  uint32_t getFrameCount();

private:
  bool _initialized;
  unsigned long _lastRender;
  unsigned long _lastFrameTime;
  float _avgFrameTime;
  uint32_t _frameCount;
  bool _forceFullRedraw;

  void _renderClock();
  void _renderTodo();
  void _renderAlarm();
  void _renderSchedule();
  void _renderSettings();
};

extern TftManager tftManager;

#endif // SIMULATION
#endif // TFT_MANAGER_H
