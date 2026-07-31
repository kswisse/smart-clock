#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class JsonUtil {
public:
  static bool parse(const char* json, StaticJsonDocument<512>& doc);
  static bool parse(const char* json, DynamicJsonDocument& doc);
  static String stringify(StaticJsonDocumentBase& doc);
  static String errorResponse(int code, const char* message);
  static String successResponse(const char* message, StaticJsonDocumentBase* data = nullptr);
  static String wrapResponse(bool success, int code, const char* message, const char* dataJson = nullptr);
};

#endif // JSON_UTIL_H
