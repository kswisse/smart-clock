#include "encoder_hal.h"

#ifndef SIMULATION
#include "../core/pin_config.h"
#include "../core/config.h"
#include "../utils/logger.h"

EncoderHAL encoderHal;

void EncoderHAL::init() {
  pinMode(PIN_POTENTIOMETER, INPUT);
  pinMode(PIN_CONTROL_BUTTON, INPUT_PULLUP);
  analogReadResolution(12);

  _position = 0;
  _lastPosition = 0;
  _lastDelta = 0;
  _lastMovementTime = 0;
  _acceleration = 1;
  _lastPotBucket = map(analogRead(PIN_POTENTIOMETER), 0, 4095, 0, POT_LOGICAL_STEPS);

  _btnRaw = false;
  _btnStable = false;
  _btnJustPressed = false;
  _btnJustReleased = false;
  _btnPressStart = 0;
  _btnLastDebounce = 0;

  logger.info("INPUT", "Potentiometer=%d, control button=%d",
              PIN_POTENTIOMETER, PIN_CONTROL_BUTTON);
}

void EncoderHAL::update() {
  unsigned long now = millis();

  // Keep the old EncoderHAL interface, but source navigation from the
  // potentiometer that exists in the schematic.
  uint16_t bucket = map(analogRead(PIN_POTENTIOMETER), 0, 4095, 0, POT_LOGICAL_STEPS);
  if (bucket != _lastPotBucket) {
    _position += (int32_t)bucket - (int32_t)_lastPotBucket;
    _acceleration = 1;
    _lastMovementTime = now;
    _lastPotBucket = bucket;
  }

  // ── Button debounce ──
  bool raw = digitalRead(PIN_CONTROL_BUTTON) == LOW;
  if (raw != _btnRaw) {
    _btnLastDebounce = now;
  }
  _btnRaw = raw;

  if ((now - _btnLastDebounce) >= BTN_DEBOUNCE_MS) {
    bool stable = _btnRaw;
    if (stable && !_btnStable) {
      _btnPressStart = now;
      _btnJustPressed = true;
    } else if (!stable && _btnStable) {
      _btnPressStart = 0;
      _btnJustReleased = true;
    }
    _btnStable = stable;
  }
}

int32_t EncoderHAL::getDelta() {
  int32_t delta = _position - _lastPosition;
  _lastPosition = _position;
  return delta;
}

int32_t EncoderHAL::getPosition() {
  return _position;
}

void EncoderHAL::resetPosition() {
  _position = 0;
  _lastPosition = 0;
}

uint8_t EncoderHAL::getAcceleration() {
  return _acceleration;
}

bool EncoderHAL::isButtonPressed() {
  return _btnStable;
}

bool EncoderHAL::isButtonJustPressed() {
  bool result = _btnJustPressed;
  _btnJustPressed = false;
  return result;
}

bool EncoderHAL::isButtonJustReleased() {
  bool result = _btnJustReleased;
  _btnJustReleased = false;
  return result;
}

uint32_t EncoderHAL::getButtonPressDuration() {
  return _btnStable && _btnPressStart ? (millis() - _btnPressStart) : 0;
}

#endif // SIMULATION
