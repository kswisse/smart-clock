#include "api_response.h"

#ifndef SIMULATION
#include "../core/config.h"

void ApiResponse::send(AsyncWebServerRequest* request, int code, bool success, const char* message, const char* dataJson) {
  DynamicJsonDocument doc(JSON_DOC_LARGE);
  doc["success"] = success;
  doc["code"] = code;
  doc["message"] = message;
  doc["timestamp"] = millis();

  if (dataJson && strlen(dataJson) > 0) {
    DynamicJsonDocument dataDoc(JSON_DOC_LARGE);
    if (!deserializeJson(dataDoc, dataJson)) {
      doc["data"].set(dataDoc.as<JsonVariantConst>());
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
  DynamicJsonDocument doc(JSON_DOC_STATUS);
  doc["success"] = success;
  doc["code"] = code;
  doc["message"] = message;
  doc["timestamp"] = millis();
  doc["data"].set(data.as<JsonVariantConst>());

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

void ApiResponse::created(AsyncWebServerRequest* request, const char* message, const char* dataJson) {
  send(request, 201, true, message, dataJson);
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
