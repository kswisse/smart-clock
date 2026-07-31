#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "../utils/logger.h"

class Repository {
public:
  bool begin();

  // Generic CRUD
  bool read(const char* path, String& output);
  bool write(const char* path, const String& data);
  bool write(const char* path, const char* data, size_t len);
  bool exists(const char* path);
  bool remove(const char* path);
  size_t fileSize(const char* path);

  // JSON operations
  bool readJson(const char* path, DynamicJsonDocument& doc);
  bool writeJson(const char* path, JsonDocument& doc);

  // Directory operations
  void listDir(const char* dir, int levels = 0);

  // Stats
  size_t totalBytes();
  size_t usedBytes();

private:
  String _getMimeType(const char* path);
};

extern Repository repository;

#endif // REPOSITORY_H
