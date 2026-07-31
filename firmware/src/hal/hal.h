#ifndef HAL_H
#define HAL_H

#include <Arduino.h>

class HAL {
public:
  static void init();
  static void update();

  // LED
  static void ledOn();
  static void ledOff();
  static void ledToggle();
  static void ledBlink(uint16_t intervalMs);

  // Battery
  static float batteryVoltage();
  static uint8_t batteryPercent();

  // Buttons
  static bool buttonPressed(uint8_t pin);
  static bool buttonHeld(uint8_t pin, uint32_t durationMs);

  // Memory
  static size_t freeHeap();
  static size_t minFreeHeap();
  static size_t heapSize();
  static float heapFragmentation();

private:
  static unsigned long _lastLedBlink;
  static bool _ledState;
  static unsigned long _buttonPressTime[3];
};

#endif // HAL_H
