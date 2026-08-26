#ifndef CLOCK_SERVER_H
#define CLOCK_SERVER_H

#include <Arduino.h>
#include <AsyncWebServer.h>

class ClockServer {
public:
  void begin();
  void handleClient();
  AsyncWebServer* getServer();

private:
  AsyncWebServer* _server;

  void _setupRoutes();
};

extern ClockServer clockServer;

#endif // CLOCK_SERVER_H
