#ifndef ARDUINO_H_STUB
#define ARDUINO_H_STUB

#ifdef SIMULATION

#include <cstdint>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <thread>

// ── Arduino Types ───────────────────────────────────────────
typedef uint8_t byte;
typedef bool boolean;
typedef uint16_t word;

#define HIGH true
#define LOW false
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

#define LEDC_CHANNEL_0 0

// ── Arduino String (must be before classes that use it) ─────
class String {
public:
  String() : _buf("") {}
  String(const char* s) : _buf(s ? s : "") {}
  String(const String& other) : _buf(other._buf) {}
  String(char c) : _buf(1, c) {}
  String(int value) : _buf(std::to_string(value)) {}
  String(unsigned int value) : _buf(std::to_string(value)) {}
  String(long value) : _buf(std::to_string(value)) {}
  String(unsigned long value) : _buf(std::to_string(value)) {}
#if SIZE_MAX != ULONG_MAX
  // size_t differs from unsigned long on this platform (e.g. Win64);
  // on LP64 Linux size_t == unsigned long and this would be a redefinition.
  String(size_t value) : _buf(std::to_string(value)) {}
#endif
  String(float value) : _buf(std::to_string(value)) {}
  String(double value) : _buf(std::to_string(value)) {}
  String& operator=(const char* s) { _buf = s ? s : ""; return *this; }
  String& operator=(const String& other) { _buf = other._buf; return *this; }
  String& operator+=(const char* s) { _buf += s ? s : ""; return *this; }
  String& operator+=(const String& other) { _buf += other._buf; return *this; }
  const char* c_str() const { return _buf.c_str(); }
  size_t length() const { return _buf.length(); }
  bool isEmpty() const { return _buf.empty(); }
  int indexOf(const String& search, int start = 0) const {
    size_t pos = _buf.find(search._buf, start);
    return (pos == std::string::npos) ? -1 : (int)pos;
  }
  int lastIndexOf(char c) const {
    size_t pos = _buf.rfind(c);
    return (pos == std::string::npos) ? -1 : (int)pos;
  }
  String substring(int from, int to = -1) const {
    if (to < 0) to = _buf.length();
    return String(_buf.substr(from, to - from).c_str());
  }
  void toLowerCase() { for (auto& c : _buf) c = tolower(c); }
  void toUpperCase() { for (auto& c : _buf) c = toupper(c); }
  bool startsWith(const String& prefix) const { return _buf.find(prefix._buf) == 0; }
  bool endsWith(const String& suffix) const {
    if (suffix._buf.length() > _buf.length()) return false;
    return _buf.compare(_buf.length() - suffix._buf.length(), suffix._buf.length(), suffix._buf) == 0;
  }
  void replace(const String& from, const String& to) {
    size_t pos = 0;
    while ((pos = _buf.find(from._buf, pos)) != std::string::npos) {
      _buf.replace(pos, from._buf.length(), to._buf);
      pos += to._buf.length();
    }
  }
  bool contains(const String& s) const { return _buf.find(s._buf) != std::string::npos; }
  char charAt(int index) const { return (index >= 0 && index < (int)_buf.size()) ? _buf[index] : 0; }
  bool equals(const String& other) const { return _buf == other._buf; }
  int toInt() const { return atoi(_buf.c_str()); }
  float toFloat() const { return atof(_buf.c_str()); }
  bool operator==(const char* s) const { return _buf == (s ? s : ""); }
  bool operator==(const String& other) const { return _buf == other._buf; }
  bool operator!=(const char* s) const { return _buf != (s ? s : ""); }
  bool operator!=(const String& other) const { return _buf != other._buf; }
  String operator+(const String& other) const { return String((_buf + other._buf).c_str()); }
  String operator+(const char* s) const { return String((_buf + (s ? s : "")).c_str()); }
  friend String operator+(const char* a, const String& b) { return String((std::string(a) + b._buf).c_str()); }
  String repeat(int count) const {
    std::string result;
    for (int i = 0; i < count; i++) result += _buf;
    return String(result.c_str());
  }
private:
  std::string _buf;
};

// ── Mock Serial ─────────────────────────────────────────────
class MockSerial {
public:
  void begin(unsigned long) {}
  void end() {}
  void print(const char* s) { printf("%s", s); }
  void print(int v) { printf("%d", v); }
  void print(unsigned int v) { printf("%u", v); }
  void print(long v) { printf("%ld", v); }
  void print(unsigned long v) { printf("%lu", v); }
  void print(float v) { printf("%.2f", v); }
  void print(double v) { printf("%.2f", v); }
  void println() { printf("\n"); }
  void println(const char* s) { printf("%s\n", s); }
  void println(int v) { printf("%d\n", v); }
  void println(unsigned int v) { printf("%u\n", v); }
  void println(long v) { printf("%ld\n", v); }
  void println(unsigned long v) { printf("%lu\n", v); }
  void println(float v) { printf("%.2f\n", v); }

  int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
  }
};

extern MockSerial Serial;

// ── Millis / Delay ──────────────────────────────────────────
unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);
void yield();

// ── GPIO Mock ───────────────────────────────────────────────
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, bool val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int value);
#define ADC_11db 3
void analogReadResolution(int bits);
void analogSetAttenuation(int atten);

// GPIO state access (for simulation control)
bool simGetPinState(uint8_t pin);
void simSetPinState(uint8_t pin, bool state);

// ── LEDC Mock ───────────────────────────────────────────────
void ledcSetup(uint8_t channel, double freq, uint8_t resolution);
void ledcAttachPin(uint8_t pin, uint8_t channel);
void ledcWrite(uint8_t channel, uint8_t duty);
void ledcWriteTone(uint8_t channel, double freq);

// ── WiFi Mock ───────────────────────────────────────────────
enum wl_status_t {
  WL_IDLE_STATUS = 0,
  WL_CONNECTED = 3,
  WL_DISCONNECTED = 6
};

enum wifi_mode_t {
  WIFI_MODE_STA = 1,
  WIFI_MODE_AP = 2,
  WIFI_MODE_APSTA = 3
};

enum system_event_id_t {
  SYSTEM_EVENT_STA_GOT_IP = 4,
  SYSTEM_EVENT_STA_DISCONNECTED = 5
};

enum wifi_auth_mode_t {
  WIFI_AUTH_OPEN = 0
};

class MockIPAddress {
public:
  MockIPAddress() : _ip(0) {}
  MockIPAddress(uint32_t ip) : _ip(ip) {}
  String toString() const { return "192.168.1.100"; }
  operator uint32_t() const { return _ip; }
private:
  uint32_t _ip;
};

class MockWiFiClass {
public:
  void mode(wifi_mode_t m) { _mode = m; }
  bool softAP(const char* ssid, const char* pass, uint8_t ch, uint8_t hidden, uint8_t max_conn) {
    _apSsid = ssid ? ssid : "";
    _apActive = true;
    return true;
  }
  MockIPAddress softAPIP() { return MockIPAddress(0xC0A80101); }
  MockIPAddress localIP() { return MockIPAddress(0xC0A80164); }
  int32_t RSSI() { return _rssi; }
  uint8_t channel() { return _channel; }
  wl_status_t status() { return _status; }
  String SSID() { return _ssid; }
  void begin(const char* ssid, const char* pass) {
    _ssid = ssid ? ssid : "";
    _status = WL_CONNECTED;
  }
  void disconnect() { _status = WL_DISCONNECTED; }
  int scanNetworks() { return 0; }
  String SSID(int i) { return ""; }
  int32_t RSSI(int i) { return 0; }
  uint8_t channel(int i) { return 0; }
  wifi_auth_mode_t encryptionType(int i) { return WIFI_AUTH_OPEN; }
  String macAddress() { return "AA:BB:CC:DD:EE:FF"; }
  void onEvent(std::function<void(system_event_id_t)> cb) { _eventCb = cb; }

  // Simulation control
  void simSetStatus(wl_status_t s) { _status = s; }
  void simSetRSSI(int32_t r) { _rssi = r; }

private:
  wifi_mode_t _mode = WIFI_MODE_STA;
  wl_status_t _status = WL_IDLE_STATUS;
  String _ssid;
  String _apSsid;
  bool _apActive = false;
  int32_t _rssi = -50;
  uint8_t _channel = 1;
  std::function<void(system_event_id_t)> _eventCb;
};

extern MockWiFiClass WiFi;

// ── ESP Mock ────────────────────────────────────────────────
class MockESPClass {
public:
  const char* getChipModel() { return "ESP32-SIM"; }
  const char* getSdkVersion() { return "sim-1.0"; }
  uint32_t getCpuFreqMHz() { return 240; }
  uint32_t getFlashChipSize() { return 4194304; }
  uint32_t getSketchSize() { return 500000; }
  uint32_t getFreeSketchSpace() { return 3600000; }
  uint32_t getFreeHeap() { return 200000; }
  uint32_t getMinFreeHeap() { return 180000; }
  uint32_t getHeapSize() { return 320000; }
  float getHeapFragmentation() { return 5.0f; }
  void restart() { printf("[SIM] ESP restart\n"); }
};

extern MockESPClass ESP;

// ── LittleFS Mock ───────────────────────────────────────────
// Real mock classes in mock_headers/LittleFS.h
// Real implementation in mock_littlefs.cpp
// LittleFS global defined in mock_littlefs.cpp

// ── FreeRTOS Stubs ─────────────────────────────────────────
typedef void* SemaphoreHandle_t;

SemaphoreHandle_t xSemaphoreCreateMutex();
int xSemaphoreTake(SemaphoreHandle_t sem, unsigned long timeout);
int xSemaphoreGive(SemaphoreHandle_t sem);
#define pdMS_TO_TICKS(ms) (ms)
#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFF

// ── ArduinoJson Stub ───────────────────────────────────────
// When SIMULATION is defined, we still need ArduinoJson.
// This stub is only for compilation without the real library.
// In practice, ArduinoJson should be installed for simulation too.

// ── Utility ─────────────────────────────────────────────────
#define ArduinoMin(a,b) ((a)<(b)?(a):(b))
#define ArduinoMax(a,b) ((a)>(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
static inline long ArduinoMap(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// 'map' as a free function — does NOT conflict with std::map template
static inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return ArduinoMap(x, in_min, in_max, out_min, out_max);
}

#define F(s) (s)
#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t*)(addr))

// ── Time Stubs ─────────────────────────────────────────────
struct tm;
bool getLocalTime(struct tm* info, uint32_t timeout_ms = 5000);
void configTime(long gmtOffset, int daylightOffset, const char* server1, const char* server2 = nullptr);
void settimeofday(const struct timeval* tv, const void* tz);

// Simulation time control
void simSetTime(int year, int month, int day, int hour, int minute, int second);
void simAdvanceTime(unsigned long ms);

#endif // SIMULATION
#endif // ARDUINO_H_STUB
