#ifdef SIMULATION

#include "../test_common.h"

void testWifi() {
  testBeginSuite("7. WiFi Service");

  // Clean up state from previous phases
  wifiService.disconnect();
  eventBus.processQueue();

  // Initial state
  WifiState state = wifiService.getState();
  testAssert(state == WIFI_IDLE, "WiFi starts in IDLE state");
  testAssert(!wifiService.isConnected(), "WiFi not connected at boot");

  // Connect
  wifiService.startConnectSTA("TestSSID", "pass123");
  state = wifiService.getState();
  testAssert(state == WIFI_STA_CONNECTING, "WiFi transitions to CONNECTING");

  // Poll until connected
  for (int i = 0; i < 20; i++) {
    simAdvanceTime(200);
    wifiService.handleEvents();
  }
  state = wifiService.getState();
  testAssert(state == WIFI_STA_CONNECTED, "WiFi transitions to CONNECTED");
  testAssert(wifiService.isConnected(), "isConnected() returns true");

  String ip = wifiService.getIP();
  testAssert(ip.length() > 0, "IP address assigned");

  // State string
  const char* stateStr = wifiService.getStateStr();
  testAssert(stateStr != nullptr, "getStateStr() returns non-null");
  testAssert(strcmp(stateStr, "CONNECTED") == 0, "State string is CONNECTED");

  // Disconnect
  wifiService.disconnect();
  testAssert(!wifiService.isConnected(), "Not connected after disconnect");
  state = wifiService.getState();
  testAssert(state == WIFI_IDLE, "State back to IDLE after disconnect");

  // Reconnect via simConnect
  wifiService.simConnect();
  state = wifiService.getState();
  testAssert(state == WIFI_STA_CONNECTING, "simConnect sets CONNECTING");
  for (int i = 0; i < 20; i++) {
    simAdvanceTime(200);
    wifiService.handleEvents();
  }
  testAssert(wifiService.isConnected(), "Connected after simConnect");

  // Sim disconnect
  eventBus.processQueue();
  wifiService.simDisconnect();
  testAssert(!wifiService.isConnected(), "Not connected after simDisconnect");
  eventBus.processQueue();

  testEndSuite();
}

#endif
