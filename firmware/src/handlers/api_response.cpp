#include "api_response.h"

#ifndef SIMULATION
#include "../core/config.h"

void ApiResponse::send(AsyncWebServerRequest* request, int code, bool success, const char* message, const char* dataJson) {
  StaticJsonDocument<512> doc;
  doc["success"] = success;
  doc["code"] = code;
  doc["message"] = message;
  doc["timestamp"] = millis();

  if (dataJson && strlen(dataJson) > 0) {
    DynamicJsonDocument dataDoc(512);
    if (!deserializeJson(dataDoc, dataJson)) {
      doc["data"] = dataDoc.as<JsonObject>();
    } else {
      doc["data"] = JsonObject();
    }
  } else {
    doc["data"] = JsonObject();
  }

  String json;
  serializeJson(doc, json);
  _sendJson(request, code, json);
}

void ApiResponse::send(AsyncWebServerRequest* request, int code, bool success, const char* message, JsonDocument& data) {
  StaticJsonDocument<512> doc;
  doc["success"] = success;
  doc["code"] = code;
  doc["message"] = message;
  doc["timestamp"] = millis();
  doc["data"] = data.as<JsonObject>();

  String json;
  serializeJson(doc, json);
  _sendJson(request, code, json);
}

void ApiResponse::ok(AsyncWebServerRequest* request, const char* message, const char* dataJson) {
  send(request, 200, true, message, dataJson);
}

void ApiResponse::ok(AsyncWebServerRequest* request, const char* message, JsonDocument& data) {
  send(request, 200, true, message, data);
}

void ApiResponse::created(AsyncWebServerRequest* request, const char* message, JsonDocument* data) {
  if (data) {
    send(request, 201, true, message, *data);
  } else {
    send(request, 201, true, message);
  }
}

void ApiResponse::badRequest(AsyncWebServerRequest* request, const char* message) {
  send(request, 400, false, message);
}

void ApiResponse::notFound(AsyncWebServerRequest* request, const char* message) {
  send(request, 404, false, message);
}

void ApiResponse::serverError(AsyncWebServerRequest* request, const char* message) {
  send(request, 500, false, message);
}

void ApiResponse::methodNotAllowed(AsyncWebServerRequest* request) {
  send(request, 405, false, "Method not allowed");
}

void ApiResponse::sendOptions(AsyncWebServerRequest* request) {
  AsyncWebServerResponse* response = request->beginResponse(200);
  _addCorsHeaders(response);
  request->send(response);
}

void ApiResponse::_sendJson(AsyncWebServerRequest* request, int code, const String& json) {
  AsyncWebServerResponse* response = request->beginResponse(code, MIME_JSON, json);
  _addCorsHeaders(response);
  request->send(response);
}

void ApiResponse::_addCorsHeaders(AsyncWebServerResponse* response) {
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  response->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

#endif // SIMULATION
