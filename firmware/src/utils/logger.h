#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel {
  LOG_NONE  = 0,
  LOG_ERROR = 1,
  LOG_WARN  = 2,
  LOG_INFO  = 3,
  LOG_DEBUG = 4,
  LOG_VERBOSE = 5
};

#ifndef LOG_LEVEL
  #ifdef CORE_DEBUG_LEVEL
    #if CORE_DEBUG_LEVEL >= 5
      #define LOG_LEVEL LOG_VERBOSE
    #elif CORE_DEBUG_LEVEL >= 4
      #define LOG_LEVEL LOG_DEBUG
    #elif CORE_DEBUG_LEVEL >= 3
      #define LOG_LEVEL LOG_INFO
    #elif CORE_DEBUG_LEVEL >= 2
      #define LOG_LEVEL LOG_WARN
    #elif CORE_DEBUG_LEVEL >= 1
      #define LOG_LEVEL LOG_ERROR
    #else
      #define LOG_LEVEL LOG_NONE
    #endif
  #else
    #define LOG_LEVEL LOG_INFO
  #endif
#endif

class Logger {
public:
  void begin(LogLevel level = LOG_LEVEL, uint32_t baud = 115200);
  void setLevel(LogLevel level);
  LogLevel getLevel();

  void error(const char* tag, const char* fmt, ...);
  void warn(const char* tag, const char* fmt, ...);
  void info(const char* tag, const char* fmt, ...);
  void debug(const char* tag, const char* fmt, ...);
  void verbose(const char* tag, const char* fmt, ...);

  void hexDump(const char* tag, const uint8_t* data, size_t len);

  // Memory diagnostics
  void logMemory(const char* tag);
  size_t getFreeHeap();
  size_t getMinFreeHeap();
  size_t getHeapSize();
  float getHeapFragmentation();

private:
  LogLevel _level;
  bool _initialized;

  void _log(LogLevel level, const char* tag, const char* fmt, va_list args);
  const char* _levelStr(LogLevel level);
};

extern Logger logger;

#endif // LOGGER_H
