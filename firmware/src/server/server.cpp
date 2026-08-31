#include "server.h"

#ifndef SIMULATION
#include "../core/config.h"
#include "../handlers/handlers.h"
#include "../handlers/todo_handlers.h"
#include "../handlers/alarm_handlers.h"
#include "../handlers/schedule_handlers.h"
#include "../handlers/sync_handler.h"
#include "../handlers/api_response.h"
#include "../utils/logger.h"
#include "../services/wifi_service.h"
#include <LittleFS.h>

ClockServer clockServer;

void ClockServer::begin() {
  _server = new AsyncWebServer(SERVER_PORT);
  _setupRoutes();
  _server->begin();
  logger.info("SERVER", "Started on port %d", SERVER_PORT);
}

void ClockServer::handleClient() {
}

AsyncWebServer* ClockServer::getServer() {
  return _server;
}

void ClockServer::_setupRoutes() {
  // ── System API ───────────────────────────────────────────────
  _server->on("/api/status", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleStatus(r);
  });
  _server->on("/api/time", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleTimeGet(r);
  });
  _server->on("/api/time", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      handlers.handleTimePost(r, d, l, i, t);
    }
  );

  // ── Todo API ─────────────────────────────────────────────────
  _server->on("/api/todo", HTTP_GET, [](AsyncWebServerRequest* r) {
    todoHandlers.handleGetAll(r);
  });
  _server->on("/api/todo", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      todoHandlers.handleCreate(r, d, l, i, t);
    }
  );
  _server->on("^\\/api\\/todo\\/([0-9]+)$", HTTP_GET,
    [](AsyncWebServerRequest* r) {
      todoHandlers.handleGetById(r, r->pathArg(0).toInt());
    }
  );
  _server->on("^\\/api\\/todo\\/([0-9]+)$", HTTP_PUT,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      todoHandlers.handleUpdate(r, r->pathArg(0).toInt(), d, l, i, t);
    }
  );
  _server->on("^\\/api\\/todo\\/([0-9]+)$", HTTP_DELETE,
    [](AsyncWebServerRequest* r) {
      todoHandlers.handleDelete(r, r->pathArg(0).toInt());
    }
  );

  // ── Alarm API ────────────────────────────────────────────────
  _server->on("/api/alarm", HTTP_GET, [](AsyncWebServerRequest* r) {
    alarmHandlers.handleGetAll(r);
  });
  _server->on("/api/alarm", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      alarmHandlers.handleCreate(r, d, l, i, t);
    }
  );
  _server->on("^\\/api\\/alarm\\/([0-9]+)$", HTTP_GET,
    [](AsyncWebServerRequest* r) {
      alarmHandlers.handleGetById(r, r->pathArg(0).toInt());
    }
  );
  _server->on("^\\/api\\/alarm\\/([0-9]+)$", HTTP_PUT,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      alarmHandlers.handleUpdate(r, r->pathArg(0).toInt(), d, l, i, t);
    }
  );
  _server->on("^\\/api\\/alarm\\/([0-9]+)$", HTTP_DELETE,
    [](AsyncWebServerRequest* r) {
      alarmHandlers.handleDelete(r, r->pathArg(0).toInt());
    }
  );

  // ── Schedule API ─────────────────────────────────────────────
  _server->on("/api/schedule", HTTP_GET, [](AsyncWebServerRequest* r) {
    scheduleHandlers.handleGetAll(r);
  });
  _server->on("/api/schedule", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      scheduleHandlers.handleCreate(r, d, l, i, t);
    }
  );
  _server->on("^\\/api\\/schedule\\/([0-9]+)$", HTTP_GET,
    [](AsyncWebServerRequest* r) {
      scheduleHandlers.handleGetById(r, r->pathArg(0).toInt());
    }
  );
  _server->on("^\\/api\\/schedule\\/([0-9]+)$", HTTP_PUT,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      scheduleHandlers.handleUpdate(r, r->pathArg(0).toInt(), d, l, i, t);
    }
  );
  _server->on("^\\/api\\/schedule\\/([0-9]+)$", HTTP_DELETE,
    [](AsyncWebServerRequest* r) {
      scheduleHandlers.handleDelete(r, r->pathArg(0).toInt());
    }
  );

  // ── Display Settings API ──────────────────────────────────────
  _server->on("/api/display", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleDisplayGet(r);
  });
  _server->on("/api/display", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      handlers.handleDisplayPost(r, d, l, i, t);
    }
  );

  // ── Sound Settings API ───────────────────────────────────────
  _server->on("/api/sound", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleSoundGet(r);
  });
  _server->on("/api/sound", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      handlers.handleSoundPost(r, d, l, i, t);
    }
  );

  // ── WiFi Management API ──────────────────────────────────────
  _server->on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleWifiGet(r);
  });
  _server->on("/api/wifi", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      handlers.handleWifiPost(r, d, l, i, t);
    }
  );

  // ── Device Info API ──────────────────────────────────────────
  _server->on("/api/device", HTTP_GET, [](AsyncWebServerRequest* r) {
    handlers.handleDeviceGet(r);
  });

  // ── Sync API ────────────────────────────────────────────────
  _server->on("/api/sync", HTTP_POST,
    [](AsyncWebServerRequest* r) {}, NULL,
    [](AsyncWebServerRequest* r, uint8_t* d, size_t l, size_t i, size_t t) {
      syncHandler.handleSyncPost(r, d, l, i, t);
    }
  );

  // ── CORS ─────────────────────────────────────────────────────
  _server->on("/api/status", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/time", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/todo", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/alarm", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/schedule", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/display", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/sound", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/wifi", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/device", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });
  _server->on("/api/sync", HTTP_OPTIONS, [](AsyncWebServerRequest* r) {
    ApiResponse::sendOptions(r);
  });

  // ── Static Files ─────────────────────────────────────────────
  _server->serveStatic("/", LittleFS, "/")
    .setDefaultFile("index.html")
    .setCacheControl("no-cache");
  _server->onNotFound([](AsyncWebServerRequest* r) {
    handlers.handleNotFound(r);
  });

  logger.info("SERVER", "Routes configured (status, time, todo, alarm, schedule, display, sound, wifi)");
}

#endif // SIMULATION
