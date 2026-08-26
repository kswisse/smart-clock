#include "handlers.h"

#ifndef SIMULATION
#include "api_response.h"
#include "../core/config.h"
#include "../core/firmware_info.h"
#include "../repositories/repository.h"
#include "../repositories/config_repo.h"
#include "../services/status_service.h"
#include "../services/time_service.h"
#include "../services/wifi_service.h"
#include <ArduinoJson.h>

Handlers handlers;

void Handlers::handleStatus(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  DynamicJsonDocument doc(JSON_DOC_STATUS);

  // Device status
  SystemStatus device = statusService.getSystemStatus();
  JsonObject dev = doc.createNestedObject("device");
  dev["firmware"] = device.firmware;
  dev["chip"] = device.chip;
  dev["flash"] = device.flashSize;
  dev["heap"] = device.freeHeap;
  dev["mac"] = device.macAddress;
  dev["battery"] = device.batteryPercent;

  // Time
  TimeInfo time = timeService.getTime();
  JsonObject t = doc.createNestedObject("time");
  t["current"] = time.current;
  t["date"] = time.date;
  t["timezone"] = time.timezone;
  t["mode"] = time.mode;

  // WiFi
  WifiStatus wifi = wifiService.getStatus();
  JsonObject w = doc.createNestedObject("wifi");
  w["connected"] = wifi.connected;
  w["ssid"] = wifi.ssid;
  w["ip"] = wifi.ip;
  w["rssi"] = wifi.rssi;

  // Display settings
  DisplaySettings display = configRepo.display();
  JsonObject dsp = doc.createNestedObject("display");
  display.toJson(dsp);

  // Sound settings
  SoundSettings sound = configRepo.sound();
  JsonObject snd = doc.createNestedObject("sound");
  sound.toJson(snd);

  // Empty collections (for future features)
  doc.createNestedArray("todos");
  doc.createNestedArray("alarms");
  doc.createNestedArray("schedule");

  // Firmware metadata
  JsonObject fw = doc.createNestedObject("firmware");
  fw["version"] = FW_VERSION;
  fw["build"] = FW_BUILD_TIME;
  fw["api"] = API_VERSION;

  ApiResponse::ok(request, "Status retrieved", doc);
}

void Handlers::handleTimeGet(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  TimeInfo time = timeService.getTime();
  StaticJsonDocument<256> doc;
  doc["current"] = time.current;
  doc["date"] = time.date;
  doc["timezone"] = time.timezone;
  doc["mode"] = time.mode;
  doc["hour"] = time.hour;
  doc["minute"] = time.minute;
  doc["second"] = time.second;

  ApiResponse::ok(request, "Time retrieved", doc);
}

void Handlers::handleTimePost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return; // Wait for complete body

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  bool success = timeService.setTimeFromJson(body);
  if (success) {
    ApiResponse::ok(request, "Time updated");
  } else {
    ApiResponse::badRequest(request, "Invalid time data");
  }
}

// ── Display Settings ──────────────────────────────────────────

void Handlers::handleDisplayGet(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  DisplaySettings display = configRepo.display();
  StaticJsonDocument<256> doc;
  JsonObject dsp = doc.to<JsonObject>();
  display.toJson(dsp);
  ApiResponse::ok(request, "Display settings retrieved", doc);
}

void Handlers::handleDisplayPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return;

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  DynamicJsonDocument doc(512);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  DisplaySettings display = configRepo.display();
  if (doc.containsKey("brightness")) display.brightness = doc["brightness"];
  if (doc.containsKey("autoDim")) display.autoDim = doc["autoDim"];
  if (doc.containsKey("timeout")) display.timeout = doc["timeout"];
  if (doc.containsKey("animation")) display.animation = doc["animation"];
  if (doc.containsKey("theme")) strncpy(display.theme, doc["theme"] | "dark", sizeof(display.theme) - 1);

  configRepo.setDisplay(display);
  configRepo.save();

  StaticJsonDocument<256> resp;
  JsonObject respObj = resp.to<JsonObject>();
  display.toJson(respObj);
  ApiResponse::ok(request, "Display settings updated", resp);
}

// ── Sound Settings ────────────────────────────────────────────

void Handlers::handleSoundGet(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  SoundSettings sound = configRepo.sound();
  StaticJsonDocument<128> doc;
  JsonObject snd = doc.to<JsonObject>();
  sound.toJson(snd);
  ApiResponse::ok(request, "Sound settings retrieved", doc);
}

void Handlers::handleSoundPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return;

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  DynamicJsonDocument doc(512);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  SoundSettings sound = configRepo.sound();
  if (doc.containsKey("alarmVolume")) sound.alarmVolume = doc["alarmVolume"];
  if (doc.containsKey("alarmSound")) strncpy(sound.alarmSound, doc["alarmSound"] | "default", sizeof(sound.alarmSound) - 1);

  configRepo.setSound(sound);
  configRepo.save();

  StaticJsonDocument<128> resp;
  JsonObject sndResp = resp.to<JsonObject>();
  sound.toJson(sndResp);
  ApiResponse::ok(request, "Sound settings updated", resp);
}

// ── WiFi Management ──────────────────────────────────────────

void Handlers::handleWifiGet(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  if (request->hasParam("scan")) {
    // Scan for networks and return wrapped in { networks: [...] }
    wifiService.scan();
    DynamicJsonDocument resp(JSON_DOC_LARGE);
    String networksJson = wifiService.scanResultToJson();
    DynamicJsonDocument scanDoc(JSON_DOC_LARGE);
    deserializeJson(scanDoc, networksJson);
    JsonArray src = scanDoc.as<JsonArray>();
    JsonArray arr = resp.createNestedArray("networks");
    for (JsonObject net : src) {
      JsonObject obj = arr.createNestedObject();
      obj["ssid"] = net["ssid"] | "";
      obj["rssi"] = net["rssi"] | 0;
      obj["channel"] = net["channel"] | 0;
      obj["secure"] = net["secure"] | false;
    }
    ApiResponse::ok(request, "WiFi scan complete", resp);
    return;
  }

  // Return current status
  WifiStatus status = wifiService.getStatus();
  StaticJsonDocument<256> doc;
  doc["connected"] = status.connected;
  doc["ssid"] = status.ssid;
  doc["ip"] = status.ip;
  doc["rssi"] = status.rssi;
  doc["state"] = wifiService.getStateStr();
  ApiResponse::ok(request, "WiFi status retrieved", doc);
}

void Handlers::handleWifiPost(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
  wifiService.touchActivity();
  if (index + len < total) return;

  char body[512];
  size_t copyLen = min(len, sizeof(body) - 1);
  memcpy(body, data, copyLen);
  body[copyLen] = '\0';

  DynamicJsonDocument doc(512);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    ApiResponse::badRequest(request, "Invalid JSON");
    return;
  }

  const char* action = doc["action"] | "";

  if (strcmp(action, "disconnect") == 0) {
    wifiService.disconnect();
    ApiResponse::ok(request, "WiFi disconnected");
    return;
  }

  // Connect
  const char* ssid = doc["ssid"] | "";
  const char* password = doc["password"] | "";

  if (strlen(ssid) == 0) {
    ApiResponse::badRequest(request, "SSID required");
    return;
  }

  wifiService.startConnectSTA(ssid, password);
  ApiResponse::ok(request, "WiFi connecting");
}

// ── Device Info ──────────────────────────────────────────────

void Handlers::handleDeviceGet(AsyncWebServerRequest* request) {
  wifiService.touchActivity();
  SystemStatus device = statusService.getSystemStatus();
  StaticJsonDocument<256> doc;
  doc["firmware"] = device.firmware;
  doc["chip"] = device.chip;
  doc["flash"] = device.flashSize;
  doc["heap"] = device.freeHeap;
  doc["mac"] = device.macAddress;
  doc["battery"] = device.batteryPercent;
  ApiResponse::ok(request, "Device info retrieved", doc);
}

void Handlers::handleStaticFile(AsyncWebServerRequest* request) {
  String path = request->url();
  if (path == "/") path = PATH_INDEX;

  if (!repository.exists(path.c_str())) {
    if (!path.endsWith(".html") && !path.endsWith(".css") &&
        !path.endsWith(".js") && !path.endsWith(".svg") &&
        !path.endsWith(".json")) {
      path += ".html";
    }
  }

  if (!repository.exists(path.c_str())) {
    handleNotFound(request);
    return;
  }

  String content;
  if (!repository.read(path.c_str(), content)) {
    ApiResponse::serverError(request, "Failed to read file");
    return;
  }

  String mimeType = "application/octet-stream";
  if (path.endsWith(".html")) mimeType = MIME_HTML;
  else if (path.endsWith(".css")) mimeType = MIME_CSS;
  else if (path.endsWith(".js")) mimeType = MIME_JS;
  else if (path.endsWith(".svg")) mimeType = MIME_SVG;
  else if (path.endsWith(".json")) mimeType = MIME_JSON;
  else if (path.endsWith(".png")) mimeType = MIME_PNG;
  else if (path.endsWith(".ico")) mimeType = MIME_ICO;

  AsyncWebServerResponse* response = request->beginResponse(200, mimeType, content);
  response->addHeader("Cache-Control", "public, max-age=3600");
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}

void Handlers::handleNotFound(AsyncWebServerRequest* request) {
  ApiResponse::notFound(request);
}

void Handlers::_sendCorsOptions(AsyncWebServerRequest* request) {
  ApiResponse::sendOptions(request);
}

#endif // SIMULATION
