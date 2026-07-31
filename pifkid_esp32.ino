// PIFKID 2026 Smart Clock - ESP32 Web Server
// Yêu cầu: cài ESP32 board, thư viện LittleFS
// Cách upload index.html lên LittleFS: Tools > ESP32 Sketch Data Upload

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// WiFi - chế độ AP (không cần router)
const char* ssid = "PIFKID-2026";
const char* password = "12345678";

WebServer server(80);

// Dữ liệu lưu trong RAM (có thể lưu vào NVS/Preferences để giữ khi mất nguồn)
String alarm_time = "06:30";
bool alarm_enabled = true;

struct ScheduleItem {
  String time;
  String task;
};
std::vector<ScheduleItem> schedule;

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, []() {
    File f = LittleFS.open("/index.html", "r");
    if (!f) { server.send(500, "text/plain", "File not found"); return; }
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/api/data", HTTP_GET, []() {
    String json = "{\"alarm_time\":\"" + alarm_time + "\",\"alarm_enabled\":";
    json += alarm_enabled ? "true" : "false";
    json += ",\"schedule\":[";
    for (size_t i = 0; i < schedule.size(); i++) {
      if (i > 0) json += ",";
      json += "{\"time\":\"" + schedule[i].time + "\",\"task\":\"" + schedule[i].task + "\"}";
    }
    json += "]}";
    server.send(200, "application/json", json);
  });

  server.on("/api/alarm", HTTP_POST, []() {
    if (!server.hasArg("plain")) { server.send(400, "text/plain", "Bad Request"); return; }
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));
    if (err) { server.send(400, "text/plain", "JSON error"); return; }
    alarm_time = doc["alarm_time"].as<String>();
    alarm_enabled = doc["enabled"].as<bool>();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
    Serial.printf("Alarm set: %s enabled=%d\n", alarm_time.c_str(), alarm_enabled);
  });

  server.on("/api/schedule", HTTP_POST, []() {
    if (!server.hasArg("plain")) { server.send(400, "text/plain", "Bad Request"); return; }
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, server.arg("plain"));
    if (err) { server.send(400, "text/plain", "JSON error"); return; }
    schedule.clear();
    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
      ScheduleItem item;
      item.time = obj["time"].as<String>();
      item.task = obj["task"].as<String>();
      schedule.push_back(item);
    }
    server.send(200, "application/json", "{\"status\":\"ok\"}");
    Serial.printf("Schedule saved: %d items\n", schedule.size());
  });

  // HEAD cho kiểm tra kết nối
  server.on("/api/data", HTTP_HEAD, []() { server.send(200); });

  server.begin();
}

void loop() {
  server.handleClient();
}
