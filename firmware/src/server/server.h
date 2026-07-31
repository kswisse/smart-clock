#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>
#include <AsyncWebServer.h>

class Server {
public:
  void begin();
  void handleClient();
  AsyncWebServer* getServer();

private:
  AsyncWebServer* _server;

  void _setupRoutes();
  void _setupCORS();
};

extern Server server;

#endif // SERVER_H
