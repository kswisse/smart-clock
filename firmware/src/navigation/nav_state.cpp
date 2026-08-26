#include "nav_state.h"

#ifndef SIMULATION
#include "../hal/encoder_hal.h"
#include "../hal/buttons_hal.h"
#include "../events/event_bus.h"
#include "../utils/logger.h"
#include "../core/config.h"

NavState navState;

void NavState::init() {
  _currentScreen = SCREEN_CLOCK;
  _previousScreen = SCREEN_CLOCK;
  _changed = false;
  _lastInputTime = 0;
  _lastEvent = {SCREEN_CLOCK, SCREEN_CLOCK, 0};
  logger.info("NAV", "Navigation init (screen=CLOCK)");
}

void NavState::update() {
  unsigned long now = millis();

  // Read encoder
  int32_t delta = encoderHal.getDelta();
  if (delta != 0) {
    handleEncoder(delta);
    _lastInputTime = now;
  }

  // Read button
  if (encoderHal.isButtonJustPressed()) {
    handleButton(true, false);
    _lastInputTime = now;
  }
  if (encoderHal.isButtonJustReleased()) {
    handleButton(false, false);
  }

  // Long press on encoder button = back to clock
  if (encoderHal.isButtonPressed() &&
      encoderHal.getButtonPressDuration() >= NAV_LONG_PRESS_MS &&
      (now - _lastInputTime) < NAV_LONG_PRESS_MS + 50) {
    navigateBack();
  }
}

ScreenType NavState::getCurrentScreen() {
  return _currentScreen;
}

void NavState::navigateTo(ScreenType screen) {
  if (screen == _currentScreen) return;
  if (screen >= SCREEN_COUNT) return;

  _previousScreen = _currentScreen;
  _currentScreen = screen;
  _changed = true;

  NavEvent event = {_previousScreen, _currentScreen, 3};
  _lastEvent = event;
  _emitNavEvent(event);

  logger.info("NAV", "Navigate: %d -> %d", _previousScreen, _currentScreen);
}

void NavState::navigateBack() {
  if (_currentScreen == SCREEN_CLOCK) return;
  navigateTo(SCREEN_CLOCK);
}

void NavState::handleEncoder(int32_t delta) {
  if (_currentScreen == SCREEN_CLOCK) {
    // On clock screen: rotate = cycle screens
    int next = ((int)_currentScreen + (delta > 0 ? 1 : -1) + SCREEN_COUNT) % SCREEN_COUNT;
    navigateTo((ScreenType)next);
  } else {
    // On content screens: rotate = scroll/select within page
    // Emit event for page-specific handling
    Event evt;
    evt.type = EVT_NAV_ENCODER;
    evt.itemId = delta;
    evt.timestamp = millis();
    eventBus.emit(evt);
  }
}

void NavState::handleButton(bool pressed, bool longPress) {
  if (_currentScreen == SCREEN_CLOCK) {
    if (pressed && !longPress) {
      navigateTo(SCREEN_TODO);
    }
  } else if (_currentScreen == SCREEN_ALARM) {
    if (longPress) {
      // Snooze alarm if playing
      Event evt;
      evt.type = EVT_SOUND_CHANGED;
      evt.itemId = 0;
      evt.message = "snooze";
      eventBus.emit(evt);
    } else {
      Event evt;
      evt.type = EVT_NAV_SELECT;
      evt.timestamp = millis();
      eventBus.emit(evt);
    }
  } else {
    if (longPress) {
      navigateBack();
    } else {
      Event evt;
      evt.type = EVT_NAV_SELECT;
      evt.timestamp = millis();
      eventBus.emit(evt);
    }
  }
}

bool NavState::hasChanged() {
  bool result = _changed;
  _changed = false;
  return result;
}

NavEvent NavState::getLastEvent() {
  return _lastEvent;
}

void NavState::_emitNavEvent(NavEvent navEvent) {
  Event evt;
  evt.type = EVT_SCREEN_CHANGED;
  evt.itemId = (uint16_t)navEvent.to;
  evt.timestamp = millis();
  eventBus.emit(evt);
}

#endif // SIMULATION