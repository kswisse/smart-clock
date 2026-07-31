#ifndef CONFIG_REPO_H
#define CONFIG_REPO_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <mutex>
#include "../models/models.h"

#define CONFIG_FILE "/config.json"
#define REPO_LOCK_TIMEOUT_MS 100

class ConfigRepository {
public:
  bool begin();
  bool load();
  bool save();

  // Display settings
  DisplaySettings display();
  void setDisplay(const DisplaySettings& settings);

  // Sound settings
  SoundSettings sound();
  void setSound(const SoundSettings& settings);

  // WiFi settings
  String wifiSSID();
  String wifiPassword();
  void setWifi(const char* ssid, const char* password);

  // Time settings
  String timezone();
  bool ntpMode();
  void setTimezone(const char* tz);
  void setNTPMode(bool enabled);

private:
  DisplaySettings _display;
  SoundSettings _sound;
  char _wifiSSID[64];
  char _wifiPassword[64];
  char _timezoneStr[64];
  bool _ntpMode;
  bool _dirty;
  SemaphoreHandle_t _mutex;

  void _defaults();
  void _lock();
  void _unlock();
};

extern ConfigRepository configRepo;

#endif // CONFIG_REPO_H
