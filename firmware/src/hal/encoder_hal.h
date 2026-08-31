#ifndef HAL_ENCODER_H
#define HAL_ENCODER_H

#include <Arduino.h>

#ifndef SIMULATION
class EncoderHAL {
public:
  void init();
  void update();

  // Position / delta
  int32_t getDelta();           // Returns accumulated ticks since last getDelta() call
  int32_t getPosition();
  void resetPosition();

  // Acceleration
  uint8_t getAcceleration();    // 1-ENCODER_ACCEL_MAX

  // Button
  bool isButtonPressed();
  bool isButtonJustPressed();
  bool isButtonJustReleased();
  uint32_t getButtonPressDuration();

private:
  volatile int32_t _position;
  int32_t _lastPosition;
  int32_t _lastDelta;
  unsigned long _lastMovementTime;
  uint8_t _acceleration;
  uint16_t _lastPotBucket;

  // Button state
  bool _btnRaw;
  bool _btnStable;
  bool _btnJustPressed;
  bool _btnJustReleased;
  unsigned long _btnPressStart;
  unsigned long _btnLastDebounce;
};

extern EncoderHAL encoderHal;

#endif // SIMULATION
#endif // HAL_ENCODER_H
