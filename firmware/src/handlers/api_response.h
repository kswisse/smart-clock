#ifndef API_RESPONSE_H
#define API_RESPONSE_H

#include <Arduino.h>
#include <AsyncWebServer.h>
#include <ArduinoJson.h>

class ApiResponse {
public:
  // Standard JSON response builders
  static void send(AsyncWebServerRequest* request, int code, bool success, const char* message, const char* dataJson = nullptr);
  static void send(AsyncWebServerRequest* request, int code, bool success, const char* message, JsonDocument& data);

  // Convenience methods
  static void ok(AsyncWebServerRequest* request, const char* message, const char* dataJson = nullptr);
  static void ok(AsyncWebServerRequest* request, const char* message, JsonDocument& data);
  static void created(AsyncWebServerRequest* request, const char* message, JsonDocument* data = nullptr);
  static void badRequest(AsyncWebServerRequest* request, const char* message = "Bad request");
  static void notFound(AsyncWebServerRequest* request, const char* message = "Not found");
  static void serverError(AsyncWebServerRequest* request, const char* message = "Internal server error");
  static void methodNotAllowed(AsyncWebServerRequest* request);

  // CORS handling
  static void sendOptions(AsyncWebServerRequest* request);

private:
  static void _sendJson(AsyncWebServerRequest* request, int code, const String& json);
  static void _addCorsHeaders(AsyncWebServerResponse* response);
};

#endif // API_RESPONSE_H
