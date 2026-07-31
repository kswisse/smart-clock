#include "logger.h"

Logger logger;

void Logger::begin(LogLevel level, uint32_t baud) {
  Serial.begin(baud);
  delay(100);
  _level = level;
  _initialized = true;

  Serial.println();
  Serial.println("╔══════════════════════════════════════════╗");
  Serial.println("║         Logger Initialized              ║");
  Serial.printf("║  Level: %-29s ║\n", _levelStr(level));
  Serial.println("╚══════════════════════════════════════════╝");
  Serial.println();
}

void Logger::setLevel(LogLevel level) {
  _level = level;
}

LogLevel Logger::getLevel() {
  return _level;
}

void Logger::error(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _log(LOG_ERROR, tag, fmt, args);
  va_end(args);
}

void Logger::warn(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _log(LOG_WARN, tag, fmt, args);
  va_end(args);
}

void Logger::info(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _log(LOG_INFO, tag, fmt, args);
  va_end(args);
}

void Logger::debug(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _log(LOG_DEBUG, tag, fmt, args);
  va_end(args);
}

void Logger::verbose(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _log(LOG_VERBOSE, tag, fmt, args);
  va_end(args);
}

void Logger::hexDump(const char* tag, const uint8_t* data, size_t len) {
  if (_level < LOG_DEBUG) return;
  char buf[8];
  Serial.printf("[%s] HEX (%d bytes): ", tag, len);
  for (size_t i = 0; i < len; i++) {
    snprintf(buf, sizeof(buf), "%02X ", data[i]);
    Serial.print(buf);
    if ((i + 1) % 16 == 0) Serial.println("\n               ");
  }
  Serial.println();
}

void Logger::logMemory(const char* tag) {
  Serial.printf("[%s] Memory: Free=%dKB, MinFree=%dKB, Size=%dKB, Frag=%.1f%%\n",
                tag,
                getFreeHeap() / 1024,
                getMinFreeHeap() / 1024,
                getHeapSize() / 1024,
                getHeapFragmentation());
}

size_t Logger::getFreeHeap() {
  return ESP.getFreeHeap();
}

size_t Logger::getMinFreeHeap() {
  return ESP.getMinFreeHeap();
}

size_t Logger::getHeapSize() {
  return ESP.getHeapSize();
}

float Logger::getHeapFragmentation() {
  return ESP.getHeapFragmentation();
}

void Logger::_log(LogLevel level, const char* tag, const char* fmt, va_list args) {
  if (level > _level || !_initialized) return;

  char prefix[4];
  switch (level) {
    case LOG_ERROR:   strcpy(prefix, "E"); break;
    case LOG_WARN:    strcpy(prefix, "W"); break;
    case LOG_INFO:    strcpy(prefix, "I"); break;
    case LOG_DEBUG:   strcpy(prefix, "D"); break;
    case LOG_VERBOSE: strcpy(prefix, "V"); break;
    default:          strcpy(prefix, "?"); break;
  }

  Serial.printf("[%s][%s] ", prefix, tag);
  char buf[256];
  vsnprintf(buf, sizeof(buf), fmt, args);
  Serial.println(buf);
}

const char* Logger::_levelStr(LogLevel level) {
  switch (level) {
    case LOG_NONE:    return "NONE";
    case LOG_ERROR:   return "ERROR";
    case LOG_WARN:    return "WARN";
    case LOG_INFO:    return "INFO";
    case LOG_DEBUG:   return "DEBUG";
    case LOG_VERBOSE: return "VERBOSE";
    default:          return "UNKNOWN";
  }
}
