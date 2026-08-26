#ifndef SYNC_HANDLER_H
#define SYNC_HANDLER_H

#include <Arduino.h>
#include <AsyncWebServer.h>

class SyncHandler {
public:
  void handleSyncPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
};

extern SyncHandler syncHandler;

#endif // SYNC_HANDLER_H
