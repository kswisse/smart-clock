#include "wifi_service.h"

#ifndef SIMULATION
#include "../core/config.h"
#include "../events/event_bus.h"
#include "../utils/logger.h"

WifiService wifiService;

// ── WiFi Event Callback (ISR-safe) ─────────────────────────────
static void wifiEventCallback(system_event_id_t event) {
  wifiService._onWifiEvent(event);
}

void WifiService::begin() {
  _apMode = false;
  _staConfigured = false;
  _lastScan = 0;
  _lastStatusCheck = 0;
  _lastReconnectAttempt = 0;
  _reconnectDelay = WIFI_RECONNECT_INITIAL_MS;
  _retryCount = 0;
  _staSSID[0] = '\0';
  _staPassword[0] = '\0';

  _status.connected = false;
  _status.ssid = "";
  _status.ip = "";
  _status.rssi = 0;
  _status.channel = 0;
  _status.state = WIFI_IDLE;
  _status.retryCount = 0;
  _status.lastAttemptMs = 0;
  _status.connectStartMs = 0;
  _status.connectedMs = 0;

  // Register WiFi event handler
  WiFi.onEvent(wifiEventCallback);

  logger.info("WIFI", "WiFi service initialized (non-blocking)");
}

bool WifiService::beginAP() {
  logger.info("WIFI", "Starting AP mode: %s", WIFI_AP_SSID);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS, WIFI_AP_CHANNEL, 0, WIFI_AP_MAX_CONN);

  _apMode = true;
  _setState(WIFI_AP_MODE);
  _updateStatus();

  logger.info("WIFI", "AP started, IP: %s", getIP().c_str());
  return true;
}

void WifiService::startConnectSTA(const char* ssid, const char* password) {
  if (ssid == NULL || strlen(ssid) == 0) {
    logger.warn("WIFI", "No SSID provided, staying in AP mode");
    return;
  }

  logger.info("WIFI", "Starting STA connection to: %s", ssid);

  // Store credentials for reconnect
  strncpy(_staSSID, ssid, sizeof(_staSSID) - 1);
  strncpy(_staPassword, password ? password : "", sizeof(_staPassword) - 1);
  _staConfigured = true;

  // If in AP mode, switch to AP+STA
  if (_apMode) {
    WiFi.mode(WIFI_AP_STA);
  } else {
    WiFi.mode(WIFI_STA);
  }

  _retryCount = 0;
  _reconnectDelay = WIFI_RECONNECT_INITIAL_MS;
  _setState(WIFI_STA_CONNECTING);
  _status.connectStartMs = millis();

  WiFi.begin(_staSSID, _staPassword);
}

void WifiService::disconnect() {
  logger.info("WIFI", "Disconnecting");
  WiFi.disconnect();
  _staConfigured = false;
  _retryCount = 0;
  _setState(WIFI_IDLE);
  _status.connected = false;
  _status.ssid = "";
  _status.ip = "";
}

void WifiService::scan() {
  if (millis() - _lastScan < WIFI_SCAN_INTERVAL_MS && _networks.size() > 0) {
    return;
  }

  logger.debug("WIFI", "Scanning networks...");
  int n = WiFi.scanNetworks();
  _networks.clear();

  for (int i = 0; i < n; i++) {
    WifiNetwork net;
    net.ssid = WiFi.SSID(i);
    net.rssi = WiFi.RSSI(i);
    net.channel = WiFi.channel(i);
    net.secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    _networks.push_back(net);
  }

  _lastScan = millis();
  logger.info("WIFI", "Found %d networks", n);
}

void WifiService::handleEvents() {
  unsigned long now = millis();

  // Periodic status check
  if (now - _lastStatusCheck >= WIFI_STATUS_CHECK_MS) {
    _updateStatus();

    // Detect connection loss
    if (_status.state == WIFI_STA_CONNECTED && WiFi.status() != WL_CONNECTED) {
      logger.warn("WIFI", "Connection lost");
      _setState(WIFI_STA_DISCONNECTED);
    }

    _lastStatusCheck = now;
  }

  // State machine processing
  switch (_status.state) {
    case WIFI_STA_CONNECTING:
      _handleConnecting();
      break;

    case WIFI_STA_DISCONNECTED:
      if (_staConfigured) {
        _setState(WIFI_RECONNECTING);
        _lastReconnectAttempt = 0; // Force immediate attempt
      }
      break;

    case WIFI_RECONNECTING:
      _handleReconnecting();
      break;

    default:
      break;
  }
}

// ── State Machine Handlers ─────────────────────────────────────

void WifiService::_handleConnecting() {
  unsigned long now = millis();

  // Check if connected
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long connectTime = now - _status.connectStartMs;
    _status.connectedMs = connectTime;
    _setState(WIFI_STA_CONNECTED);
    _updateStatus();

    logger.info("WIFI", "Connected to %s in %lums, IP: %s",
                _staSSID, connectTime, getIP().c_str());

    // Emit connected event
    eventBus.emit(EVT_WIFI_CONNECTED, 0, _staSSID);
    return;
  }

  // Check timeout
  if (now - _status.connectStartMs >= WIFI_CONNECT_TIMEOUT_MS) {
    logger.warn("WIFI", "Connection timeout after %lums", WIFI_CONNECT_TIMEOUT_MS);
    WiFi.disconnect();

    _retryCount++;
    _status.retryCount = _retryCount;

    if (_retryCount >= WIFI_MAX_RETRY_COUNT) {
      logger.error("WIFI", "Max retries (%d) reached", WIFI_MAX_RETRY_COUNT);
      _setState(WIFI_CONNECTION_FAILED);
    } else {
      _setState(WIFI_RECONNECTING);
      _lastReconnectAttempt = 0; // Force immediate backoff
    }
  }
}

void WifiService::_handleReconnecting() {
  unsigned long now = millis();

  // Wait for backoff delay
  if (now - _lastReconnectAttempt < _reconnectDelay) {
    return;
  }

  _lastReconnectAttempt = now;
  _status.lastAttemptMs = now;

  logger.info("WIFI", "Reconnect attempt %d/%d (delay=%lums)",
              _retryCount + 1, WIFI_MAX_RETRY_COUNT, _reconnectDelay);

  _setState(WIFI_STA_CONNECTING);
  _status.connectStartMs = now;

  // Start connection attempt
  WiFi.begin(_staSSID, _staPassword);

  // Update backoff for next attempt
  _reconnectDelay = _calculateBackoff();
}

// ── Event Handler (called from WiFi task) ──────────────────────

void WifiService::_onWifiEvent(system_event_id_t event) {
  // Note: This runs in WiFi task context, not Arduino loop
  // Only set flags, don't do heavy work here
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      // Will be detected in handleEvents() via WiFi.status()
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      // Will be detected in handleEvents() via WiFi.status()
      break;

    default:
      break;
  }
}

// ── State Management ───────────────────────────────────────────

void WifiService::_setState(WifiState newState) {
  WifiState oldState = _status.state;
  _status.state = newState;

  if (oldState != newState) {
    logger.info("WIFI", "State: %s -> %s", getStateStr(), getStateStr());

    // Emit state change events
    switch (newState) {
      case WIFI_STA_CONNECTED:
        // Already emitted in _handleConnecting
        break;

      case WIFI_STA_DISCONNECTED:
        eventBus.emit(EVT_WIFI_DISCONNECTED, 0, "disconnected");
        break;

      case WIFI_CONNECTION_FAILED:
        eventBus.emit(EVT_WIFI_DISCONNECTED, 0, "failed");
        break;

      default:
        break;
    }
  }
}

// ── Status & Queries ───────────────────────────────────────────

WifiStatus WifiService::getStatus() {
  _updateStatus();
  return _status;
}

bool WifiService::isConnected() {
  return _status.state == WIFI_STA_CONNECTED;
}

String WifiService::getIP() {
  if (_status.state == WIFI_AP_MODE || _status.state == WIFI_IDLE) {
    return WiFi.softAPIP().toString();
  }
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }
  return WiFi.softAPIP().toString();
}

WifiState WifiService::getState() {
  return _status.state;
}

const char* WifiService::getStateStr() {
  switch (_status.state) {
    case WIFI_IDLE:              return "IDLE";
    case WIFI_AP_MODE:           return "AP_MODE";
    case WIFI_STA_CONNECTING:    return "STA_CONNECTING";
    case WIFI_STA_CONNECTED:     return "STA_CONNECTED";
    case WIFI_STA_DISCONNECTED:  return "STA_DISCONNECTED";
    case WIFI_RECONNECTING:      return "RECONNECTING";
    case WIFI_CONNECTION_FAILED: return "CONNECTION_FAILED";
    default:                     return "UNKNOWN";
  }
}

// ── JSON Serialization ─────────────────────────────────────────

String WifiService::statusToJson() {
  _updateStatus();
  StaticJsonDocument<512> doc;
  doc["connected"] = _status.connected;
  doc["ssid"] = _status.ssid;
  doc["ip"] = _status.ip;
  doc["rssi"] = _status.rssi;
  doc["channel"] = _status.channel;
  doc["state"] = getStateStr();
  doc["retryCount"] = _status.retryCount;
  doc["uptime"] = _status.connectedMs;

  String json;
  serializeJson(doc, json);
  return json;
}

String WifiService::scanResultToJson() {
  scan();
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  JsonArray arr = doc.to<JsonArray>();

  for (const auto& net : _networks) {
    JsonObject obj = arr.createNestedObject();
    obj["ssid"] = net.ssid;
    obj["rssi"] = net.rssi;
    obj["channel"] = net.channel;
    obj["secure"] = net.secure;
  }

  String json;
  serializeJson(doc, json);
  return json;
}

// ── Private Helpers ────────────────────────────────────────────

void WifiService::_updateStatus() {
  _status.connected = WiFi.status() == WL_CONNECTED;
  _status.ssid = _status.connected ? WiFi.SSID() : "";
  _status.ip = getIP();
  _status.rssi = WiFi.RSSI();
  _status.channel = WiFi.channel();
  _status.retryCount = _retryCount;
}

uint8_t WifiService::_rssiToQuality(int32_t rssi) {
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return 2 * (rssi + 100);
}

unsigned long WifiService::_calculateBackoff() {
  // Exponential backoff with jitter
  unsigned long delay = _reconnectDelay * WIFI_RECONNECT_MULTIPLIER;

  // Cap at max
  if (delay > WIFI_RECONNECT_MAX_MS) {
    delay = WIFI_RECONNECT_MAX_MS;
  }

  // Add jitter (±20%)
  unsigned long jitter = delay * WIFI_RECONNECT_JITTER_PCT / 100;
  unsigned long offset = random(0, jitter * 2 + 1);
  delay = delay - jitter + offset;

  // Ensure minimum
  if (delay < WIFI_RECONNECT_INITIAL_MS) {
    delay = WIFI_RECONNECT_INITIAL_MS;
  }

  return delay;
}

#endif // SIMULATION
