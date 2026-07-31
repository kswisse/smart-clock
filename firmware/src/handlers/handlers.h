#ifndef HANDLERS_H
#define HANDLERS_H

#include <Arduino.h>
#include <AsyncWebServer.h>

class Handlers {
public:
  // API handlers
  void handleStatus(AsyncWebServerRequest* request);
  void handleTimeGet(AsyncWebServerRequest* request);
  void handleTimePost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);

  // Display settings
  void handleDisplayGet(AsyncWebServerRequest* request);
  void handleDisplayPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);

  // Sound settings
  void handleSoundGet(AsyncWebServerRequest* request);
  void handleSoundPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);

  // WiFi management
  void handleWifiGet(AsyncWebServerRequest* request);
  void handleWifiPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);

  // Device info
  void handleDeviceGet(AsyncWebServerRequest* request);

  // Static file serving
  void handleStaticFile(AsyncWebServerRequest* request);

  // Error handlers
  void handleNotFound(AsyncWebServerRequest* request);

private:
  void _sendCorsOptions(AsyncWebServerRequest* request);
};

extern Handlers handlers;

#endif // HANDLERS_H
