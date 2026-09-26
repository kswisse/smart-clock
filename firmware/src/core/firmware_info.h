#ifndef FIRMWARE_INFO_H
#define FIRMWARE_INFO_H

#include <Arduino.h>

struct FirmwareInfo {
  char version[16];
  char chipModel[32];
  char sdkVersion[32];
  char cpuFreq[16];
  char flashSize[16];
  char macAddress[20];
  uint32_t freeHeap;
  uint32_t uptime;
};

FirmwareInfo firmwareInfoGet();

#endif // FIRMWARE_INFO_H
