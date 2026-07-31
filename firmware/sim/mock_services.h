// ──────────────────────────────────────────────────────────────
// Mock Services Header — Class declarations for simulation
// ──────────────────────────────────────────────────────────────
#ifndef MOCK_SERVICES_H
#define MOCK_SERVICES_H

#ifdef SIMULATION

#include "arduino_stubs.h"
#include "../src/core/config.h"
#include "../src/events/event_bus.h"
#include <cstdio>
#include <cstring>

// ════════════════════════════════════════════════════════════════
// WifiState
// ════════════════════════════════════════════════════════════════
enum WifiState {
  WIFI_IDLE,
  WIFI_AP_MODE,
  WIFI_STA_CONNECTING,
  WIFI_STA_CONNECTED,
  WIFI_STA_DISCONNECTED,
  WIFI_RECONNECTING,
  WIFI_CONNECTION_FAILED
};

// ════════════════════════════════════════════════════════════════
// WifiStatus
// ════════════════════════════════════════════════════════════════
struct WifiStatus {
  bool connected;
  String ssid;
  String ip;
  int32_t rssi;
  int32_t channel;
  WifiState state;
  uint8_t retryCount;
  unsigned long lastAttemptMs;
  unsigned long connectStartMs;
  unsigned long connectedMs;
};

// ════════════════════════════════════════════════════════════════
// MockWifiService
// ════════════════════════════════════════════════════════════════
class MockWifiService {
public:
  void begin() {
    _status.connected = false;
    _status.state = WIFI_IDLE;
    _status.retryCount = 0;
    printf("[SIM] WiFi: service initialized\n");
  }

  bool beginAP() {
    _status.state = WIFI_AP_MODE;
    _apMode = true;
    printf("[SIM] WiFi: AP started SSID=%s\n", WIFI_AP_SSID);
    return true;
  }

  void startConnectSTA(const char* ssid, const char* password) {
    _status.state = WIFI_STA_CONNECTING;
    _status.connectStartMs = millis();
    _status.ssid = ssid ? ssid : "";
    printf("[SIM] WiFi: connecting to %s...\n", ssid);
  }

  void disconnect() {
    _status.connected = false;
    _status.state = WIFI_IDLE;
    printf("[SIM] WiFi: disconnected\n");
  }

  void scan() { printf("[SIM] WiFi: scan (no networks in simulation)\n"); }

  void handleEvents() {
    if (_status.state == WIFI_STA_CONNECTING) {
      if (millis() - _status.connectStartMs > 500) {
        _status.state = WIFI_STA_CONNECTED;
        _status.connected = true;
        _status.ip = "192.168.1.100";
        _status.connectedMs = millis() - _status.connectStartMs;
        printf("[SIM] WiFi: CONNECTED ip=%s in %lums\n",
               _status.ip.c_str(), _status.connectedMs);
        eventBus.emit(EVT_WIFI_CONNECTED, 0, _status.ssid.c_str());
      }
    }
  }

  WifiStatus getStatus() { return _status; }
  bool isConnected() { return _status.connected; }
  String getIP() { return _status.ip; }
  WifiState getState() { return _status.state; }
  const char* getStateStr() {
    switch (_status.state) {
      case WIFI_IDLE: return "IDLE";
      case WIFI_AP_MODE: return "AP_MODE";
      case WIFI_STA_CONNECTING: return "CONNECTING";
      case WIFI_STA_CONNECTED: return "CONNECTED";
      case WIFI_STA_DISCONNECTED: return "DISCONNECTED";
      case WIFI_RECONNECTING: return "RECONNECTING";
      case WIFI_CONNECTION_FAILED: return "FAILED";
      default: return "UNKNOWN";
    }
  }

  String statusToJson() { return "{}"; }
  String scanResultToJson() { return "[]"; }

  void simConnect() {
    _status.state = WIFI_STA_CONNECTING;
    _status.connectStartMs = millis();
  }
  void simDisconnect() {
    _status.connected = false;
    _status.state = WIFI_STA_DISCONNECTED;
    printf("[SIM] WiFi: simulated disconnect\n");
    eventBus.emit(EVT_WIFI_DISCONNECTED, 0, "disconnected");
  }

private:
  WifiStatus _status = {};
  bool _apMode = false;
};

// ════════════════════════════════════════════════════════════════
// TimeInfo
// ════════════════════════════════════════════════════════════════
struct TimeInfo {
  String current;
  String date;
  char timezone[64];
  char mode[16];
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

// ════════════════════════════════════════════════════════════════
// MockTimeService
// ════════════════════════════════════════════════════════════════
class MockTimeService {
public:
  void begin() { printf("[SIM] Time: service initialized\n"); }

  bool beginNTP(const char* server, long gmtOffset, int daylightOffset) {
    strncpy(_info.mode, "ntp", sizeof(_info.mode));
    printf("[SIM] Time: NTP started server=%s\n", server);
    return true;
  }

  void setManual(int year, int month, int day, int hour, int minute, int second) {
    simSetTime(year, month, day, hour, minute, second);
    strncpy(_info.mode, "manual", sizeof(_info.mode));
    printf("[SIM] Time: manual set %04d-%02d-%02d %02d:%02d:%02d\n",
           year, month, day, hour, minute, second);
  }

  void setMode(const char* mode) {
    strncpy(_info.mode, mode, sizeof(_info.mode));
  }

  void setTimezone(const char* tz) {
    strncpy(_info.timezone, tz, sizeof(_info.timezone));
  }

  TimeInfo getTime() {
    _refreshTime();
    return _info;
  }

  void update() {
    if (millis() - _lastUpdate >= 1000) {
      _refreshTime();
      _lastUpdate = millis();
    }
  }

  String timeToJson() { return "{}"; }
  bool setTimeFromJson(const char* json) { return true; }

  unsigned long getUnixTime() { return millis() / 1000; }
  bool isNtpSynced() { return _ntpSynced; }
  unsigned long getTimeSinceSync() { return millis() - _lastNtpSync; }

  void saveTimezone(const char* tz) { setTimezone(tz); }
  String loadTimezone() { return String(_info.timezone); }

  void simSync() {
    _ntpSynced = true;
    _lastNtpSync = millis();
    printf("[SIM] Time: NTP synced\n");
  }

private:
  TimeInfo _info = {};
  bool _ntpSynced = false;
  unsigned long _lastUpdate = 0;
  unsigned long _lastNtpSync = 0;

  void _refreshTime() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {
      _info.hour = timeinfo.tm_hour;
      _info.minute = timeinfo.tm_min;
      _info.second = timeinfo.tm_sec;
      _info.day = timeinfo.tm_mday;
      _info.month = timeinfo.tm_mon + 1;
      _info.year = timeinfo.tm_year + 1900;

      char buf[8];
      snprintf(buf, sizeof(buf), "%02d:%02d", _info.hour, _info.minute);
      _info.current = buf;

      snprintf(buf, sizeof(buf), "%04d-%02d-%02d", _info.year, _info.month, _info.day);
      _info.date = buf;
    }
  }
};

// ── Global mock service instance declarations ────────────────
extern MockWifiService wifiService;
extern MockTimeService timeService;

#endif // SIMULATION
#endif // MOCK_SERVICES_H
