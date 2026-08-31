#include "hal.h"
#include "../core/config.h"
#include "../utils/logger.h"

unsigned long HAL::_lastLedBlink = 0;
bool HAL::_ledState = false;
unsigned long HAL::_buttonPressTime[3] = {0, 0, 0};

void HAL::init() {
  #ifdef PIN_LED_STATUS
  pinMode(PIN_LED_STATUS, OUTPUT);
  #endif
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);

  pinMode(PIN_POTENTIOMETER, INPUT);

  #ifdef PIN_BATTERY_ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  #endif

  // Backlight pin
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);

  logger.info("HAL", "Hardware initialized");
}

void HAL::update() {
  // Auto LED blink handled externally via ledBlink()
}

void HAL::ledOn() {
  #ifdef PIN_LED_STATUS
  digitalWrite(PIN_LED_STATUS, HIGH);
  #endif
}

void HAL::ledOff() {
  #ifdef PIN_LED_STATUS
  digitalWrite(PIN_LED_STATUS, LOW);
  #endif
}

void HAL::ledToggle() {
  #ifdef PIN_LED_STATUS
  _ledState = !_ledState;
  digitalWrite(PIN_LED_STATUS, _ledState ? HIGH : LOW);
  #endif
}

void HAL::ledBlink(uint16_t intervalMs) {
  #ifdef PIN_LED_STATUS
  if (millis() - _lastLedBlink >= intervalMs) {
    ledToggle();
    _lastLedBlink = millis();
  }
  #else
  (void)intervalMs;
  #endif
}

float HAL::batteryVoltage() {
  #ifdef PIN_BATTERY_ADC
  int raw = analogRead(PIN_BATTERY_ADC);
  return (raw / (float)BATTERY_ADC_MAX) * 3.3 * BATTERY_DIVIDER_RATIO;
  #else
  return 0.0;
  #endif
}

uint8_t HAL::batteryPercent() {
  float voltage = batteryVoltage();
  if (voltage >= BATTERY_VOLTAGE_MAX) return 100;
  if (voltage <= BATTERY_VOLTAGE_MIN) return 0;
  float percent = (voltage - BATTERY_VOLTAGE_MIN) /
                  (BATTERY_VOLTAGE_MAX - BATTERY_VOLTAGE_MIN) * 100.0;
  return (uint8_t)constrain(percent, 0.0f, 100.0f);
}

bool HAL::buttonPressed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

bool HAL::buttonHeld(uint8_t pin, uint32_t durationMs) {
  if (digitalRead(pin) == LOW) {
    int idx = (pin == PIN_BUTTON_1) ? 0 : (pin == PIN_BUTTON_2) ? 1 : 2;
    if (_buttonPressTime[idx] == 0) {
      _buttonPressTime[idx] = millis();
    } else if (millis() - _buttonPressTime[idx] >= durationMs) {
      _buttonPressTime[idx] = 0;
      return true;
    }
  } else {
    int idx = (pin == PIN_BUTTON_1) ? 0 : (pin == PIN_BUTTON_2) ? 1 : 2;
    _buttonPressTime[idx] = 0;
  }
  return false;
}

size_t HAL::freeHeap() {
  return ESP.getFreeHeap();
}

size_t HAL::minFreeHeap() {
  return ESP.getMinFreeHeap();
}

size_t HAL::heapSize() {
  return ESP.getHeapSize();
}

float HAL::heapFragmentation() {
  return 0.0f;
}
