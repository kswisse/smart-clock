#ifndef TFT_STATUS_BAR_H
#define TFT_STATUS_BAR_H

#include <Arduino.h>

#ifndef SIMULATION
class TftStatusBar {
public:
  void init();
  void render();
  void updateTime(const char* timeStr);
  void updateWifi(bool connected, int rssi);
  void updateBattery(uint8_t percent);
  void invalidate();

private:
  bool _initialized;
  bool _dirty;
  char _time[12];
  bool _wifiConnected;
  int _wifiRssi;
  uint8_t _batteryPercent;
  unsigned long _lastRender;

  void _drawWifiIcon(int x, int y, bool connected, int rssi);
  void _drawBatteryIcon(int x, int y, uint8_t percent);
};

extern TftStatusBar tftStatusBar;

#endif // SIMULATION
#endif // TFT_STATUS_BAR_H