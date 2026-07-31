#ifndef STATUS_SERVICE_H
#define STATUS_SERVICE_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct SystemStatus {
  String firmware;
  String chip;
  String flashSize;
  String freeHeap;
  String macAddress;
  float batteryVoltage;
  uint8_t batteryPercent;
  uint32_t uptime;
  bool littlefsOk;
};

class StatusService {
public:
  void begin();
  void update();
  SystemStatus getSystemStatus();
  String statusToJson();

private:
  SystemStatus _status;
  unsigned long _lastUpdate;

  float _readBatteryVoltage();
  uint8_t _voltageToPercent(float voltage);
};

extern StatusService statusService;

#endif // STATUS_SERVICE_H
