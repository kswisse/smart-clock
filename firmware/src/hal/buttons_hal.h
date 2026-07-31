#ifndef HAL_BUTTONS_H
#define HAL_BUTTONS_H

#include <Arduino.h>

#ifndef SIMULATION
struct ButtonState {
  bool pressed;        // Currently held down
  bool justPressed;    // Rising edge (transition from up to down)
  bool justReleased;   // Falling edge (transition from down to up)
  bool longPressed;    // Held >= LONG_PRESS_MS
  uint32_t pressDuration; // How long held (ms), 0 if not pressed
};
#endif // SIMULATION

class ButtonsHAL {
public:
  void init();
  void update();
  ButtonState getState(uint8_t pin);
  bool isAnyPressed();

private:
  static const uint8_t NUM_BUTTONS = 2;  // BTN1 + BTN2 only (encoder btn owned by EncoderHAL)
  bool _rawState[NUM_BUTTONS];
  bool _lastStable[NUM_BUTTONS];
  bool _lastEdge[NUM_BUTTONS];      // For justPressed/justReleased
  unsigned long _pressStart[NUM_BUTTONS];
  unsigned long _lastDebounce[NUM_BUTTONS];

  uint8_t _pinToIndex(uint8_t pin);
  uint8_t _indexToPin(uint8_t idx);
};

extern ButtonsHAL buttonsHAL;

#endif // HAL_BUTTONS_H
