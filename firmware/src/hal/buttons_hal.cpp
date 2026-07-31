#include "buttons_hal.h"

#ifndef SIMULATION
#include "../core/pin_config.h"
#include "../core/config.h"
#include "../utils/logger.h"

ButtonsHAL buttonsHAL;

void ButtonsHAL::init() {
  uint8_t pins[NUM_BUTTONS] = {PIN_BUTTON_1, PIN_BUTTON_2};
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    pinMode(pins[i], INPUT_PULLUP);
    _rawState[i] = false;
    _lastStable[i] = false;
    _lastEdge[i] = false;
    _pressStart[i] = 0;
    _lastDebounce[i] = 0;
  }
  logger.info("BUTTONS", "Buttons init (%d pins)", NUM_BUTTONS);
}

void ButtonsHAL::update() {
  unsigned long now = millis();
  uint8_t pins[NUM_BUTTONS] = {PIN_BUTTON_1, PIN_BUTTON_2};

  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    bool raw = digitalRead(pins[i]) == LOW;

    // Debounce
    if (raw != _rawState[i]) {
      _lastDebounce[i] = now;
    }
    _rawState[i] = raw;

    if ((now - _lastDebounce[i]) < BTN_DEBOUNCE_MS) continue;

    bool stable = _rawState[i];

    // Edge detection
    if (stable && !_lastStable[i]) {
      // Just pressed
      _pressStart[i] = now;
      _lastEdge[i] = true;  // Set justPressed flag
    } else if (!stable && _lastStable[i]) {
      // Just released
      _pressStart[i] = 0;
      _lastEdge[i] = true;  // Set justReleased flag
    }

    _lastStable[i] = stable;
  }
}

ButtonState ButtonsHAL::getState(uint8_t pin) {
  ButtonState state = {false, false, false, false, 0};
  uint8_t idx = _pinToIndex(pin);
  if (idx >= NUM_BUTTONS) return state;

  state.pressed = _lastStable[idx];

  if (_lastEdge[idx]) {
    if (state.pressed) {
      state.justPressed = true;
    } else {
      state.justReleased = true;
    }
    _lastEdge[idx] = false;  // Consume the edge
  }

  state.pressDuration = _pressStart[idx] ? (millis() - _pressStart[idx]) : 0;
  state.longPressed = state.pressed && state.pressDuration >= NAV_LONG_PRESS_MS;

  return state;
}

bool ButtonsHAL::isAnyPressed() {
  return _lastStable[0] || _lastStable[1];
}

uint8_t ButtonsHAL::_pinToIndex(uint8_t pin) {
  if (pin == PIN_BUTTON_1) return 0;
  if (pin == PIN_BUTTON_2) return 1;
  return 0xFF;
}

uint8_t ButtonsHAL::_indexToPin(uint8_t idx) {
  uint8_t pins[NUM_BUTTONS] = {PIN_BUTTON_1, PIN_BUTTON_2};
  return (idx < NUM_BUTTONS) ? pins[idx] : 0;
}

#endif // SIMULATION
