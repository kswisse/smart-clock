#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <Arduino.h>
#include <time.h>

#ifndef SIMULATION
struct TimeInfo {
  String current;    // HH:MM
  String date;       // YYYY-MM-DD or formatted
  char timezone[64];
  char mode[16];     // "ntp" or "manual"
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

class TimeService {
public:
  bool beginNTP(const char* server, long gmtOffset, int daylightOffset);
  void setManual(int year, int month, int day, int hour, int minute, int second);
  void setMode(const char* mode);
  void setTimezone(const char* tz);
  TimeInfo getTime();
  void update();

  // JSON serialization
  String timeToJson();
  bool setTimeFromJson(const char* json);

  // Enhanced methods
  unsigned long getUnixTime();
  bool isNtpSynced();
  unsigned long getTimeSinceSync();
  void saveTimezone(const char* tz);
  String loadTimezone();

private:
  TimeInfo _info;
  bool _ntpActive;
  bool _manualMode;
  unsigned long _lastUpdate;
  char _timezone[64];
  struct tm _timeinfo;

  unsigned long _lastNtpSync;
  bool _wasSynced;
  char _savedTimezone[64];

  void _refreshTime();
  String _formatDate(int year, int month, int day);
  String _formatTime(int hour, int minute);
};

extern TimeService timeService;

#endif // SIMULATION
#endif // TIME_SERVICE_H
