#include "repository.h"
#include "../core/config.h"

Repository repository;

bool Repository::begin() {
  if (!LittleFS.begin(true)) {
    logger.error("REPO", "LittleFS mount failed");
    return false;
  }
  logger.info("REPO", "LittleFS mounted: %dKB / %dKB",
              usedBytes() / 1024, totalBytes() / 1024);
  return true;
}

bool Repository::read(const char* path, String& output) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    logger.debug("REPO", "Read failed: %s", path);
    return false;
  }
  output = file.readString();
  file.close();
  return true;
}

bool Repository::write(const char* path, const String& data) {
  return write(path, data.c_str(), data.length());
}

bool Repository::write(const char* path, const char* data, size_t len) {
  File file = LittleFS.open(path, "w");
  if (!file) {
    logger.error("REPO", "Write failed: %s", path);
    return false;
  }
  file.write((const uint8_t*)data, len);
  file.close();
  logger.debug("REPO", "Written %d bytes to %s", len, path);
  return true;
}

bool Repository::exists(const char* path) {
  return LittleFS.exists(path);
}

bool Repository::remove(const char* path) {
  return LittleFS.remove(path);
}

size_t Repository::fileSize(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) return 0;
  size_t size = file.size();
  file.close();
  return size;
}

bool Repository::readJson(const char* path, DynamicJsonDocument& doc) {
  String content;
  if (!read(path, content)) return false;

  DeserializationError err = deserializeJson(doc, content);
  if (err) {
    logger.error("REPO", "JSON parse error in %s: %s", path, err.c_str());
    return false;
  }
  return true;
}

bool Repository::writeJson(const char* path, JsonDocument& doc) {
  String output;
  serializeJson(doc, output);
  return write(path, output);
}

void Repository::listDir(const char* dir, int levels) {
  Dir root = LittleFS.openDir(dir);
  while (root.next()) {
    logger.debug("REPO", "%s/%s (%d bytes)", dir, root.fileName().c_str(), root.fileSize());
    if (root.isDirectory() && levels > 0) {
      String subPath = String(dir) + "/" + root.fileName();
      listDir(subPath.c_str(), levels - 1);
    }
  }
}

size_t Repository::totalBytes() {
  return LittleFS.totalBytes();
}

size_t Repository::usedBytes() {
  return LittleFS.usedBytes();
}

String Repository::_getMimeType(const char* path) {
  String p = String(path);
  if (p.endsWith(".html")) return MIME_HTML;
  if (p.endsWith(".css"))  return MIME_CSS;
  if (p.endsWith(".js"))   return MIME_JS;
  if (p.endsWith(".svg"))  return MIME_SVG;
  if (p.endsWith(".json")) return MIME_JSON;
  if (p.endsWith(".png"))  return MIME_PNG;
  if (p.endsWith(".ico"))  return MIME_ICO;
  return "application/octet-stream";
}
