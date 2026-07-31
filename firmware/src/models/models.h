#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>
#include <ArduinoJson.h>

// ── Todo ─────────────────────────────────────────────────────
struct Todo {
  uint16_t id;
  char title[64];
  char description[128];
  char color[12];
  bool completed;
  unsigned long createdAt;

  void clear() {
    id = 0;
    title[0] = '\0';
    description[0] = '\0';
    strcpy(color, "#4fc3f7");
    completed = false;
    createdAt = 0;
  }

  void toJson(JsonObject& obj) const {
    obj["id"] = id;
    obj["title"] = title;
    obj["description"] = description;
    obj["color"] = color;
    obj["completed"] = completed;
    obj["created_at"] = createdAt;
  }

  bool fromJson(const JsonObject& obj) {
    if (obj.containsKey("id")) id = obj["id"];
    if (obj.containsKey("title")) strncpy(title, obj["title"] | "", sizeof(title) - 1);
    if (obj.containsKey("description")) strncpy(description, obj["description"] | "", sizeof(description) - 1);
    if (obj.containsKey("color")) strncpy(color, obj["color"] | "#4fc3f7", sizeof(color) - 1);
    if (obj.containsKey("completed")) completed = obj["completed"];
    return true;
  }
};

// ── Alarm ────────────────────────────────────────────────────
struct Alarm {
  uint16_t id;
  uint8_t hour;
  uint8_t minute;
  bool repeatDays[7];  // Sun-Sat
  bool enabled;
  char sound[16];
  uint8_t volume;

  void clear() {
    id = 0;
    hour = 0;
    minute = 0;
    for (int i = 0; i < 7; i++) repeatDays[i] = false;
    enabled = true;
    strcpy(sound, "default");
    volume = 50;
  }

  void toJson(JsonObject& obj) const {
    obj["id"] = id;
    obj["hour"] = hour;
    obj["minute"] = minute;
    JsonArray days = obj.createNestedArray("repeat");
    for (int i = 0; i < 7; i++) {
      if (repeatDays[i]) days.add(i);
    }
    obj["enabled"] = enabled;
    obj["sound"] = sound;
    obj["volume"] = volume;
  }

  bool fromJson(const JsonObject& obj) {
    if (obj.containsKey("id")) id = obj["id"];
    if (obj.containsKey("hour")) hour = obj["hour"];
    if (obj.containsKey("minute")) minute = obj["minute"];
    if (obj.containsKey("repeat")) {
      JsonArray arr = obj["repeat"].as<JsonArray>();
      for (int i = 0; i < 7; i++) repeatDays[i] = false;
      for (int i = 0; i < arr.size() && i < 7; i++) {
        int day = arr[i];
        if (day >= 0 && day < 7) repeatDays[day] = true;
      }
    }
    if (obj.containsKey("enabled")) enabled = obj["enabled"];
    if (obj.containsKey("sound")) strncpy(sound, obj["sound"] | "default", sizeof(sound) - 1);
    if (obj.containsKey("volume")) volume = obj["volume"];
    return true;
  }
};

// ── Schedule Entry ───────────────────────────────────────────
struct ScheduleEntry {
  uint16_t id;
  uint8_t day;       // 0=Sun, 1=Mon, ..., 6=Sat
  char startTime[6]; // HH:MM
  char endTime[6];   // HH:MM
  char title[64];
  char color[12];

  void clear() {
    id = 0;
    day = 0;
    memset(startTime, 0, sizeof(startTime));
    memset(endTime, 0, sizeof(endTime));
    title[0] = '\0';
    strcpy(color, "#4fc3f7");
  }

  void toJson(JsonObject& obj) const {
    obj["id"] = id;
    obj["day"] = day;
    obj["start"] = startTime;
    obj["end"] = endTime;
    obj["title"] = title;
    obj["color"] = color;
  }

  bool fromJson(const JsonObject& obj) {
    if (obj.containsKey("id")) id = obj["id"];
    if (obj.containsKey("day")) day = obj["day"];
    if (obj.containsKey("start")) strncpy(startTime, obj["start"] | "00:00", sizeof(startTime) - 1);
    if (obj.containsKey("end")) strncpy(endTime, obj["end"] | "00:00", sizeof(endTime) - 1);
    if (obj.containsKey("title")) strncpy(title, obj["title"] | "", sizeof(title) - 1);
    if (obj.containsKey("color")) strncpy(color, obj["color"] | "#4fc3f7", sizeof(color) - 1);
    return true;
  }
};

// ── Display Settings ─────────────────────────────────────────
struct DisplaySettings {
  uint8_t brightness;   // 0-100
  bool autoDim;
  char theme[16];
  uint16_t timeout;     // seconds, 0=never
  bool animation;

  void defaults() {
    brightness = 80;
    autoDim = false;
    strcpy(theme, "dark");
    timeout = 30;
    animation = true;
  }

  void toJson(JsonObject& obj) const {
    obj["brightness"] = brightness;
    obj["autoDim"] = autoDim;
    obj["theme"] = theme;
    obj["timeout"] = timeout;
    obj["animation"] = animation;
  }

  bool fromJson(const JsonObject& obj) {
    if (obj.containsKey("brightness")) brightness = obj["brightness"];
    if (obj.containsKey("autoDim")) autoDim = obj["autoDim"];
    if (obj.containsKey("theme")) strncpy(theme, obj["theme"] | "dark", sizeof(theme) - 1);
    if (obj.containsKey("timeout")) timeout = obj["timeout"];
    if (obj.containsKey("animation")) animation = obj["animation"];
    return true;
  }
};

// ── Sound Settings ───────────────────────────────────────────
struct SoundSettings {
  uint8_t alarmVolume;  // 0-100
  char alarmSound[16];

  void defaults() {
    alarmVolume = 50;
    strcpy(alarmSound, "default");
  }

  void toJson(JsonObject& obj) const {
    obj["alarmVolume"] = alarmVolume;
    obj["alarmSound"] = alarmSound;
  }

  bool fromJson(const JsonObject& obj) {
    if (obj.containsKey("alarmVolume")) alarmVolume = obj["alarmVolume"];
    if (obj.containsKey("alarmSound")) strncpy(alarmSound, obj["alarmSound"] | "default", sizeof(alarmSound) - 1);
    return true;
  }
};

#endif // MODELS_H
