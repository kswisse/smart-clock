#include "tft_status_bar.h"

#ifndef SIMULATION
#include "display_hal.h"
#include "../core/config.h"
#include "../core/pin_config.h"
#include "../utils/logger.h"

// WiFi signal icon (small)
static const uint8_t WIFI_ICON_ON[] PROGMEM = {
  0b00000100, 0b00101010, 0b01010101, 0b10101010
};

TftStatusBar tftStatusBar;

void TftStatusBar::init() {
  _initialized = true;
  _dirty = true;
  strcpy(_time, "--:--");
  _wifiConnected = false;
  _wifiRssi = 0;
  _batteryPercent = 0;
  _lastRender = 0;
  logger.info("TFT_BAR", "Status bar init");
}

void TftStatusBar::render() {
  if (!_initialized) return;
  if (!_dirty && (millis() - _lastRender < STATUS_REDRAW_MS)) return;

  uint16_t barColor = 0x2104;  // Dark gray
  uint16_t textColor = 0xFFFF; // White

  // Background
  displayHAL.fillRect(0, 0, DISPLAY_WIDTH, STATUS_BAR_HEIGHT, barColor);
  displayHAL.drawLine(0, STATUS_BAR_HEIGHT - 1, DISPLAY_WIDTH, STATUS_BAR_HEIGHT - 1, 0x4208);

  // Time (left)
  displayHAL.printAt(4, 4, _time, 2, textColor, barColor);

  // WiFi icon (right side)
  _drawWifiIcon(DISPLAY_WIDTH - 50, 4, _wifiConnected, _wifiRssi);

  // Battery icon (rightmost)
  _drawBatteryIcon(DISPLAY_WIDTH - 20, 4, _batteryPercent);

  _dirty = false;
  _lastRender = millis();
}

void TftStatusBar::updateTime(const char* timeStr) {
  if (strcmp(_time, timeStr) != 0) {
    strncpy(_time, timeStr, sizeof(_time) - 1);
    _dirty = true;
  }
}

void TftStatusBar::updateWifi(bool connected, int rssi) {
  if (_wifiConnected != connected || _wifiRssi != rssi) {
    _wifiConnected = connected;
    _wifiRssi = rssi;
    _dirty = true;
  }
}

void TftStatusBar::updateBattery(uint8_t percent) {
  if (_batteryPercent != percent) {
    _batteryPercent = percent;
    _dirty = true;
  }
}

void TftStatusBar::invalidate() {
  _dirty = true;
}

void TftStatusBar::_drawWifiIcon(int x, int y, bool connected, int rssi) {
  uint16_t color = connected ? 0x07E0 : 0xF800;  // Green or Red
  if (connected) {
    // Simple wifi arcs
    displayHAL.drawCircle(x + 10, y + 12, 10, color);
    displayHAL.drawCircle(x + 10, y + 12, 6, color);
    displayHAL.fillCircle(x + 10, y + 12, 2, color);
  } else {
    // X icon
    displayHAL.drawLine(x + 2, y + 2, x + 18, y + 18, color);
    displayHAL.drawLine(x + 18, y + 2, x + 2, y + 18, color);
  }
}

void TftStatusBar::_drawBatteryIcon(int x, int y, uint8_t percent) {
  uint16_t fillColor = (percent > 20) ? 0x07E0 : 0xF800;
  // Battery outline
  displayHAL.drawRect(x, y + 2, 14, 10, 0xFFFF);
  displayHAL.fillRect(x + 14, y + 5, 2, 4, 0xFFFF);  // Tab
  // Fill level
  uint8_t fillW = map(percent, 0, 100, 0, 12);
  if (fillW > 0) {
    displayHAL.fillRect(x + 1, y + 3, fillW, 8, fillColor);
  }
}

#endif // SIMULATION