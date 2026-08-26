#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <vector>

#ifndef SIMULATION
// ── WiFi State Machine ─────────────────────────────────────────
enum WifiState {
  WIFI_IDLE,
  WIFI_AP_MODE,
  WIFI_STA_CONNECTING,
  WIFI_STA_CONNECTED,
  WIFI_STA_DISCONNECTED,
  WIFI_RECONNECTING,
  WIFI_CONNECTION_FAILED
};

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

struct WifiNetwork {
  String ssid;
  int32_t rssi;
  uint8_t channel;
  bool secure;
};

class WifiService {
public:
  void begin();
  bool beginAP();
  void stopAP();
  void startConnectSTA(const char* ssid, const char* password);
  void disconnect();
  void scan();
  void handleEvents();
  WifiStatus getStatus();
  void _onWifiEvent(arduino_event_id_t event);
  bool isConnected();
  bool isAPActive();
  String getIP();

  // Activity tracking for AP timeout
  void touchActivity();
  bool isAPTimeout();

  // JSON serialization
  String statusToJson();
  String scanResultToJson();

  // State machine
  WifiState getState();
  const char* getStateStr();

private:
  WifiStatus _status;
  std::vector<WifiNetwork> _networks;
  bool _apMode;
  unsigned long _lastScan;
  unsigned long _lastStatusCheck;
  unsigned long _lastReconnectAttempt;
  unsigned long _reconnectDelay;
  uint8_t _retryCount;
  bool _staConfigured;
  char _staSSID[64];
  char _staPassword[64];
  unsigned long _lastActivityMs;

  void _setState(WifiState newState);
  void _updateStatus();
  void _handleConnecting();
  void _handleReconnecting();
  uint8_t _rssiToQuality(int32_t rssi);
  unsigned long _calculateBackoff();
};

extern WifiService wifiService;

#endif // SIMULATION
#endif // WIFI_SERVICE_H
