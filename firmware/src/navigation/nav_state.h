#ifndef NAV_STATE_H
#define NAV_STATE_H

#include <Arduino.h>

#ifndef SIMULATION
enum ScreenType {
  SCREEN_CLOCK = 0,
  SCREEN_TODO,
  SCREEN_ALARM,
  SCREEN_SCHEDULE,
  SCREEN_SETTINGS,
  SCREEN_COUNT
};

struct NavEvent {
  ScreenType from;
  ScreenType to;
  uint8_t action;  // 0=rotate, 1=press, 2=longpress, 3=back
};

class NavState {
public:
  void init();
  void update();

  ScreenType getCurrentScreen();
  void navigateTo(ScreenType screen);
  void navigateBack();
  void handleEncoder(int32_t delta);
  void handleButton(bool pressed, bool longPress);

  bool hasChanged();
  NavEvent getLastEvent();

private:
  ScreenType _currentScreen;
  ScreenType _previousScreen;
  bool _changed;
  NavEvent _lastEvent;
  unsigned long _lastInputTime;

  void _emitNavEvent(NavEvent event);
};

extern NavState navState;

#endif // SIMULATION
#endif // NAV_STATE_H