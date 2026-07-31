#include "config_repo.h"
#include "repository.h"
#include "../core/config.h"

ConfigRepository configRepo;

bool ConfigRepository::begin() {
  _mutex = xSemaphoreCreateMutex();
  if (_mutex == NULL) {
    logger.error("CONFIG", "Mutex creation failed");
    return false;
  }
  _defaults();
  return load();
}

bool ConfigRepository::load() {
  _lock();
  DynamicJsonDocument doc(1024);
  if (!repository.readJson(CONFIG_FILE, doc)) {
    logger.warn("CONFIG", "No config file, using defaults");
    _defaults();
    _unlock();
    return false;
  }

  if (doc.containsKey("display")) {
    _display.fromJson(doc["display"].as<JsonObject>());
  }
  if (doc.containsKey("sound")) {
    _sound.fromJson(doc["sound"].as<JsonObject>());
  }
  if (doc.containsKey("wifi")) {
    JsonObject wifi = doc["wifi"].as<JsonObject>();
    strncpy(_wifiSSID, wifi["ssid"] | "", sizeof(_wifiSSID) - 1);
    strncpy(_wifiPassword, wifi["password"] | "", sizeof(_wifiPassword) - 1);
  }
  if (doc.containsKey("timezone")) {
    strncpy(_timezoneStr, doc["timezone"] | "UTC", sizeof(_timezoneStr) - 1);
  }
  if (doc.containsKey("ntpMode")) {
    _ntpMode = doc["ntpMode"];
  }

  _dirty = false;
  logger.info("CONFIG", "Config loaded");
  _unlock();
  return true;
}

bool ConfigRepository::save() {
  _lock();
  DynamicJsonDocument doc(1024);

  JsonObject disp = doc.createNestedObject("display");
  _display.toJson(disp);

  JsonObject snd = doc.createNestedObject("sound");
  _sound.toJson(snd);

  JsonObject wifi = doc.createNestedObject("wifi");
  wifi["ssid"] = _wifiSSID;
  wifi["password"] = _wifiPassword;

  doc["timezone"] = _timezoneStr;
  doc["ntpMode"] = _ntpMode;

  bool ok = repository.writeJson(CONFIG_FILE, doc);
  if (ok) {
    _dirty = false;
    logger.info("CONFIG", "Config saved");
  }
  _unlock();
  return ok;
}

DisplaySettings ConfigRepository::display() {
  _lock();
  DisplaySettings copy = _display;
  _unlock();
  return copy;
}

void ConfigRepository::setDisplay(const DisplaySettings& settings) {
  _lock();
  _display = settings;
  _dirty = true;
  _unlock();
}

SoundSettings ConfigRepository::sound() {
  _lock();
  SoundSettings copy = _sound;
  _unlock();
  return copy;
}

void ConfigRepository::setSound(const SoundSettings& settings) {
  _lock();
  _sound = settings;
  _dirty = true;
  _unlock();
}

String ConfigRepository::wifiSSID() {
  _lock();
  String ssid = String(_wifiSSID);
  _unlock();
  return ssid;
}

String ConfigRepository::wifiPassword() {
  _lock();
  String pass = String(_wifiPassword);
  _unlock();
  return pass;
}

void ConfigRepository::setWifi(const char* ssid, const char* password) {
  _lock();
  strncpy(_wifiSSID, ssid ? ssid : "", sizeof(_wifiSSID) - 1);
  strncpy(_wifiPassword, password ? password : "", sizeof(_wifiPassword) - 1);
  _dirty = true;
  _unlock();
}

String ConfigRepository::timezone() {
  _lock();
  String tz = String(_timezoneStr);
  _unlock();
  return tz;
}

bool ConfigRepository::ntpMode() {
  _lock();
  bool mode = _ntpMode;
  _unlock();
  return mode;
}

void ConfigRepository::setTimezone(const char* tz) {
  _lock();
  strncpy(_timezoneStr, tz ? tz : "UTC", sizeof(_timezoneStr) - 1);
  _dirty = true;
  _unlock();
}

void ConfigRepository::setNTPMode(bool enabled) {
  _lock();
  _ntpMode = enabled;
  _dirty = true;
  _unlock();
}

void ConfigRepository::_defaults() {
  _display.defaults();
  _sound.defaults();
  _wifiSSID[0] = '\0';
  _wifiPassword[0] = '\0';
  strcpy(_timezoneStr, "UTC");
  _ntpMode = true;
  _dirty = false;
}

void ConfigRepository::_lock() {
  xSemaphoreTake(_mutex, pdMS_TO_TICKS(REPO_LOCK_TIMEOUT_MS));
}

void ConfigRepository::_unlock() {
  xSemaphoreGive(_mutex);
}
