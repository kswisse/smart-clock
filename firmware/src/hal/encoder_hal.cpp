#include "encoder_hal.h"

#ifndef SIMULATION
#include "../core/pin_config.h"
#include "../core/config.h"
#include "../utils/logger.h"

EncoderHAL encoderHal;

void EncoderHAL::init() {
  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  pinMode(PIN_ENCODER_BTN, INPUT_PULLUP);

  _position = 0;
  _lastPosition = 0;
  _lastDelta = 0;
  _lastMovementTime = 0;
  _acceleration = 1;
  _lastPinA = digitalRead(PIN_ENCODER_A);
  _lastPinB = digitalRead(PIN_ENCODER_B);

  _btnRaw = false;
  _btnStable = false;
  _btnJustPressed = false;
  _btnJustReleased = false;
  _btnPressStart = 0;
  _btnLastDebounce = 0;

  logger.info("ENCODER", "Encoder init (A=%d B=%d BTN=%d)",
              PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_BTN);
}

void EncoderHAL::update() {
  unsigned long now = millis();

  // ── Rotary encoder reading ──
  uint8_t pinA = digitalRead(PIN_ENCODER_A);
  uint8_t pinB = digitalRead(PIN_ENCODER_B);

  if (pinA != _lastPinA) {
    int8_t direction = (pinA == pinB) ? 1 : -1;
    _position += direction;

    // Acceleration
    unsigned long elapsed = now - _lastMovementTime;
    if (elapsed < ENCODER_ACCEL_THRESHOLD) {
      _acceleration = min((uint8_t)(_acceleration + 1), (uint8_t)ENCODER_ACCEL_MAX);
    } else {
      _acceleration = 1;
    }
    _lastMovementTime = now;
  }
  _lastPinA = pinA;
  _lastPinB = pinB;

  // ── Button debounce ──
  bool raw = digitalRead(PIN_ENCODER_BTN) == LOW;
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
