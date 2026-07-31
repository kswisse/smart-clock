#include "status_service.h"
#include "../core/config.h"
#include "../core/firmware_info.h"
#include "../hal/hal.h"
#include "../utils/logger.h"
#include <WiFi.h>
#include <ESP.h>

StatusService statusService;

void StatusService::begin() {
  _status.firmware = FW_VERSION;
  _status.chip = ESP.getChipModel();
  _status.flashSize = String(ESP.getFlashChipSize() / (1024 * 1024)) + "MB";
  _status.macAddress = WiFi.macAddress();
  _status.littlefsOk = true;

  logger.info("STATUS", "Service initialized: %s", FW_VERSION);
}

void StatusService::update() {
  if (millis() - _lastUpdate < 10000) return;

  _status.freeHeap = String(HAL::freeHeap() / 1024) + "KB";
  _status.uptime = millis() / 1000;
  _status.batteryVoltage = HAL::batteryVoltage();
  _status.batteryPercent = HAL::batteryPercent();

  _lastUpdate = millis();
}

SystemStatus StatusService::getSystemStatus() {
  update();
  return _status;
}

String StatusService::statusToJson() {
  update();
  StaticJsonDocument<512> doc;
  doc["firmware"] = _status.firmware;
  doc["chip"] = _status.chip;
  doc["flash"] = _status.flashSize;
  doc["heap"] = _status.freeHeap;
  doc["mac"] = _status.macAddress;
  doc["battery"] = _status.batteryPercent;
  doc["battery_voltage"] = _status.batteryVoltage;
  doc["uptime"] = _status.uptime;

  String json;
  serializeJson(doc, json);
  return json;
}

float StatusService::_readBatteryVoltage() {
  return HAL::batteryVoltage();
}

uint8_t StatusService::_voltageToPercent(float voltage) {
  return HAL::batteryPercent();
}
