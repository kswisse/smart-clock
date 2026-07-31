// ──────────────────────────────────────────────────────────────
// Mock HAL Header — Full class declarations for simulation
// ──────────────────────────────────────────────────────────────
#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#ifdef SIMULATION

#include "arduino_stubs.h"
#include "mock_types.h"
#include "../src/core/config.h"
#include "../src/events/event_bus.h"
#include "../src/models/models.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <mutex>

// ── MockDisplayHAL ──────────────────────────────────────────
class MockDisplayHAL {
public:
  void init() { printf("[SIM] Display: init %dx%d rot=%d\n", TFT_WIDTH, TFT_HEIGHT, TFT_ROTATION); }
  void clear() { printf("[SIM] Display: clear\n"); }
  void clearRect(int x, int y, int w, int h) {}
  void fillRect(int x, int y, int w, int h, uint16_t color) {}
  void drawRect(int x, int y, int w, int h, uint16_t color) {}
  void fillCircle(int x, int y, int r, uint16_t color) {}
  void drawCircle(int x, int y, int r, uint16_t color) {}
  void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {}
  void setTextColor(uint16_t fg, uint16_t bg) {}
  void setTextSize(uint8_t size) {}
  void setCursor(int x, int y) {}
  void print(const char* text) {}
  void print(int value) {}
  void printAt(int x, int y, const char* text, uint8_t size, uint16_t fg, uint16_t bg) {
    printf("[SIM] Display: printAt(%d,%d,'%s',%d)\n", x, y, text, size);
  }
  void setBrightness(uint8_t percent) { printf("[SIM] Display: brightness=%d%%\n", percent); }
  void update() {}
  void sleep() { printf("[SIM] Display: sleep\n"); }
  void wake() { printf("[SIM] Display: wake\n"); }
  bool isInitialized() { return true; }
  uint16_t width() { return TFT_WIDTH; }
  uint16_t height() { return TFT_HEIGHT; }
private:
  bool _initialized = false;
  uint8_t _brightness = 180;
};

// ── MockButtonsHAL ──────────────────────────────────────────
class MockButtonsHAL {
public:
  void init() {
    _pins[0] = PIN_BUTTON_1;
    _pins[1] = PIN_BUTTON_2;
    for (int i = 0; i < 2; i++) {
      _lastStable[i] = false;
      _lastEdge[i] = false;
      _pressStart[i] = 0;
      _lastDebounce[i] = 0;
      _rawState[i] = false;
    }
    printf("[SIM] Buttons: init (2 pins: %d, %d)\n", PIN_BUTTON_1, PIN_BUTTON_2);
  }

  void update() {
    unsigned long now = millis();
    for (int i = 0; i < 2; i++) {
      bool raw = digitalRead(_pins[i]) == LOW;
      if (raw != _rawState[i]) _lastDebounce[i] = now;
      _rawState[i] = raw;
      if ((now - _lastDebounce[i]) < BTN_DEBOUNCE_MS) continue;
      bool stable = _rawState[i];
      if (stable && !_lastStable[i]) {
        _pressStart[i] = now;
        _lastEdge[i] = true;
      } else if (!stable && _lastStable[i]) {
        _pressStart[i] = 0;
        _lastEdge[i] = true;
      }
      _lastStable[i] = stable;
    }
  }

  ButtonState getState(uint8_t pin) {
    ButtonState state = {false, false, false, false, 0};
    int idx = -1;
    if (pin == _pins[0]) idx = 0;
    else if (pin == _pins[1]) idx = 1;
    if (idx < 0) return state;

    state.pressed = _lastStable[idx];
    if (_lastEdge[idx]) {
      state.justPressed = state.pressed;
      state.justReleased = !state.pressed;
      _lastEdge[idx] = false;
    }
    state.pressDuration = _pressStart[idx] ? (millis() - _pressStart[idx]) : 0;
    state.longPressed = state.pressed && state.pressDuration >= NAV_LONG_PRESS_MS;
    return state;
  }

  bool isAnyPressed() { return _lastStable[0] || _lastStable[1]; }

  void simPress(uint8_t pin) { simSetPinState(pin, LOW); }
  void simRelease(uint8_t pin) { simSetPinState(pin, HIGH); }

private:
  uint8_t _pins[2];
  bool _rawState[2] = {false, false};
  bool _lastStable[2] = {false, false};
  bool _lastEdge[2] = {false, false};
  unsigned long _pressStart[2] = {0, 0};
  unsigned long _lastDebounce[2] = {0, 0};
};

// ── MockEncoderHAL ──────────────────────────────────────────
class MockEncoderHAL {
public:
  void init() {
    _position = 0;
    _lastPosition = 0;
    _acceleration = 1;
    _btnStable = false;
    _btnJustPressed = false;
    _btnJustReleased = false;
    printf("[SIM] Encoder: init (A=%d B=%d BTN=%d)\n",
           PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_BTN);
  }

  void update() {
    uint8_t pinA = digitalRead(PIN_ENCODER_A);
    uint8_t pinB = digitalRead(PIN_ENCODER_B);
    if (pinA != _lastPinA) {
      int8_t dir = (pinA == pinB) ? 1 : -1;
      _position += dir;
      unsigned long elapsed = millis() - _lastMovementTime;
      if (elapsed < ENCODER_ACCEL_THRESHOLD) {
        _acceleration = (uint8_t)std::min((int)(_acceleration + 1), (int)ENCODER_ACCEL_MAX);
      } else {
        _acceleration = 1;
      }
      _lastMovementTime = millis();
    }
    _lastPinA = pinA;
    _lastPinB = pinB;

    bool raw = digitalRead(PIN_ENCODER_BTN) == LOW;
    if (raw != _btnRaw) _btnLastDebounce = millis();
    _btnRaw = raw;
    if ((millis() - _btnLastDebounce) >= BTN_DEBOUNCE_MS) {
      bool stable = _btnRaw;
      if (stable && !_btnStable) { _btnPressStart = millis(); _btnJustPressed = true; }
      else if (!stable && _btnStable) { _btnPressStart = 0; _btnJustReleased = true; }
      _btnStable = stable;
    }
  }

  int32_t getDelta() {
    int32_t delta = _position - _lastPosition;
    _lastPosition = _position;
    return delta;
  }
  int32_t getPosition() { return _position; }
  void resetPosition() { _position = 0; _lastPosition = 0; }
  uint8_t getAcceleration() { return _acceleration; }

  bool isButtonPressed() { return _btnStable; }
  bool isButtonJustPressed() { bool r = _btnJustPressed; _btnJustPressed = false; return r; }
  bool isButtonJustReleased() { bool r = _btnJustReleased; _btnJustReleased = false; return r; }
  uint32_t getButtonPressDuration() {
    return _btnStable && _btnPressStart ? (millis() - _btnPressStart) : 0;
  }

  void simRotate(int steps) {
    for (int i = 0; i < abs(steps); i++) {
      if (steps > 0) {
        simSetPinState(PIN_ENCODER_A, HIGH);
        simSetPinState(PIN_ENCODER_B, HIGH);
        simSetPinState(PIN_ENCODER_A, LOW);
      } else {
        simSetPinState(PIN_ENCODER_A, HIGH);
        simSetPinState(PIN_ENCODER_B, LOW);
        simSetPinState(PIN_ENCODER_A, LOW);
      }
      delay(1);
    }
    simSetPinState(PIN_ENCODER_A, HIGH);
    simSetPinState(PIN_ENCODER_B, HIGH);
  }

  void simPressButton() { simSetPinState(PIN_ENCODER_BTN, LOW); }
  void simReleaseButton() { simSetPinState(PIN_ENCODER_BTN, HIGH); }

private:
  int32_t _position = 0;
  int32_t _lastPosition = 0;
  uint8_t _acceleration = 1;
  uint8_t _lastPinA = HIGH;
  uint8_t _lastPinB = HIGH;
  unsigned long _lastMovementTime = 0;

  bool _btnStable = false;
  bool _btnJustPressed = false;
  bool _btnJustReleased = false;
  bool _btnRaw = false;
  unsigned long _btnPressStart = 0;
  unsigned long _btnLastDebounce = 0;
};

// ── MockTftStatusBar ────────────────────────────────────────
class MockTftStatusBar {
public:
  void init() { printf("[SIM] TftStatusBar: init\n"); }
  void render() {}
  void updateTime(const char* t) {}
  void updateWifi(bool c, int r) {}
  void updateBattery(uint8_t p) {}
  void invalidate() {}
};

// ── MockTftClock ────────────────────────────────────────────
class MockTftClock {
public:
  void init() {}
  void render(uint8_t h, uint8_t m, uint8_t s) {}
  void renderDate(uint16_t y, uint8_t mo, uint8_t d, uint8_t dow) {}
  void invalidate() {}
};

// ── MockTftTodo ─────────────────────────────────────────────
class MockTftTodo {
public:
  void init() {}
  void render(const std::vector<Todo>& t, int p) {}
  void renderItem(const Todo& t, int y) {}
  void renderHeader(int p) {}
  void clear() {}
};

// ── MockTftSchedule ─────────────────────────────────────────
class MockTftSchedule {
public:
  void init() {}
  void renderToday(const std::vector<ScheduleEntry>& e) {}
  void renderWeek(const std::vector<ScheduleEntry>& e) {}
  void setSelectedIndex(int i) {}
  void invalidate() {}
};

// ── MockTftManager ──────────────────────────────────────────
class MockTftManager {
public:
  void init() { printf("[SIM] TftManager: init\n"); }
  void update() {}
  void renderCurrentScreen() {}
  void forceRedraw() {}
  void onScreenChanged(ScreenType from, ScreenType to) {}
  unsigned long getLastFrameTime() { return 0; }
  float getAverageFrameTime() { return 0; }
  uint32_t getFrameCount() { return 0; }
};

// ── Global mock instances ───────────────────────────────────
extern MockDisplayHAL displayHAL;
extern MockButtonsHAL buttonsHAL;
extern MockEncoderHAL encoderHal;
extern MockTftStatusBar tftStatusBar;
extern MockTftClock tftClock;
extern MockTftTodo tftTodo;
extern MockTftSchedule tftSchedule;
extern MockTftManager tftManager;

// ── MockNavState ────────────────────────────────────────────
class MockNavState {
public:
  void begin() { _screen = SCREEN_CLOCK; }
  void update() {}
  ScreenType getCurrentScreen() { return _screen; }
private:
  ScreenType _screen;
};

extern MockNavState navState;

#endif // SIMULATION
#endif // MOCK_HAL_H
