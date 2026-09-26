#include "firmware_info.h"
#include "config.h"
#include <ESP.h>
#include <WiFi.h>

FirmwareInfo firmwareInfoGet() {
  FirmwareInfo info;

  strncpy(info.version, FW_VERSION, sizeof(info.version) - 1);
  info.version[sizeof(info.version) - 1] = '\0';

  strncpy(info.chipModel, ESP.getChipModel(), sizeof(info.chipModel) - 1);
  info.chipModel[sizeof(info.chipModel) - 1] = '\0';

  strncpy(info.sdkVersion, ESP.getSdkVersion(), sizeof(info.sdkVersion) - 1);
  info.sdkVersion[sizeof(info.sdkVersion) - 1] = '\0';

  snprintf(info.cpuFreq, sizeof(info.cpuFreq), "%lu MHz", ESP.getCpuFreqMHz());

  snprintf(info.flashSize, sizeof(info.flashSize), "%lu KB", ESP.getFlashChipSize() / 1024);

  strncpy(info.macAddress, WiFi.macAddress().c_str(), sizeof(info.macAddress) - 1);
  info.macAddress[sizeof(info.macAddress) - 1] = '\0';

  info.freeHeap = ESP.getFreeHeap();
  info.uptime = millis() / 1000;

  return info;
}
