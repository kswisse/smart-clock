#ifndef ASYNCWEBSERVER_H_MOCK
#define ASYNCWEBSERVER_H_MOCK
#ifdef SIMULATION

#include <cstdint>
#include <cstring>
#include <string>
#include <functional>

// Minimal AsyncWebServer stubs
class AsyncWebServerRequest;
class AsyncWebServerResponse;

enum HTTPMethod { HTTP_GET = 0, HTTP_POST, HTTP_PUT, HTTP_DELETE, HTTP_OPTIONS };

typedef std::function<void(AsyncWebServerRequest*)> ArRequestHandlerFunction;
typedef std::function<void(AsyncWebServerRequest*, uint8_t*, size_t, size_t, size_t)> ArRequestBodyFunction;

class AsyncWebServerResponse {
public:
  void addHeader(const char*, const char*) {}
  void setCode(int) {}
  void setContentLength(size_t) {}
  void setContentType(const char*) {}
  void print(const char*) {}
};

class AsyncWebServerRequest {
public:
  String url() { return ""; }
  bool hasParam(const char*) { return false; }
  AsyncWebServerRequest* getParam(const char*) { return nullptr; }
  AsyncWebServerRequest* getParam(int) { return nullptr; }
  String pathArg(int) { return ""; }
  AsyncWebServerResponse* beginResponse(int, const char*, const String&) { return new AsyncWebServerResponse(); }
  AsyncWebServerResponse* beginResponse(int) { return new AsyncWebServerResponse(); }
  void send(AsyncWebServerResponse*) {}
  void send(int) {}
};

class AsyncWebServer {
public:
  AsyncWebServer(int port) {}
  void on(const char*, HTTPMethod, ArRequestHandlerFunction) {}
  void on(const char*, HTTPMethod, ArRequestHandlerFunction, ArRequestBodyFunction, ArRequestBodyFunction) {}
  void onNotFound(ArRequestHandlerFunction) {}
  void begin() {}
  void serveStatic(const char*, class FS&, const char*) {}
};

// Mock FS class for serveStatic
class FSMock {
public:
};

// AsyncTCP stub (not used directly in simulation)
#endif // SIMULATION
#endif // ASYNCWEBSERVER_H_MOCK
