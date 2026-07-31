#include "json_util.h"
#include "../utils/logger.h"

bool JsonUtil::parse(const char* json, StaticJsonDocument<512>& doc) {
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    logger.error("JSON", "Parse error: %s", err.c_str());
    return false;
  }
  return true;
}

bool JsonUtil::parse(const char* json, DynamicJsonDocument& doc) {
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    logger.error("JSON", "Parse error: %s", err.c_str());
    return false;
  }
  return true;
}

String JsonUtil::stringify(StaticJsonDocumentBase& doc) {
  String output;
  serializeJson(doc, output);
  return output;
}

String JsonUtil::wrapResponse(bool success, int code, const char* message, const char* dataJson) {
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

  return stringify(doc);
}

String JsonUtil::errorResponse(int code, const char* message) {
  return wrapResponse(false, code, message);
}

String JsonUtil::successResponse(const char* message, StaticJsonDocumentBase* data) {
  if (data) {
    String dataStr = stringify(*data);
    return wrapResponse(true, 200, message, dataStr.c_str());
  }
  return wrapResponse(true, 200, message);
}
