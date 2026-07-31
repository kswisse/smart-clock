#ifdef SIMULATION

#include "arduino_stubs.h"
#include <chrono>
#include <map>
#include <mutex>

// ── Global Instances ────────────────────────────────────────
MockSerial Serial;
MockWiFiClass WiFi;
MockESPClass ESP;
// LittleFS global defined in mock_littlefs.cpp (LittleFSImpl type)

// ── Time Tracking ───────────────────────────────────────────
static unsigned long _simMillis = 0;
static std::mutex _timeMutex;

// Simulated wall-clock time
static struct tm _simTime = {0};
static bool _simTimeSet = false;

unsigned long millis() {
  return _simMillis;
}

unsigned long micros() {
  return _simMillis * 1000;
}

void delay(unsigned long ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void yield() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

// ── GPIO Mock ───────────────────────────────────────────────
static std::map<uint8_t, bool> _pinStates;
static std::map<uint8_t, uint8_t> _pinModes;
static std::map<uint8_t, int> _analogValues;
static std::mutex _gpioMutex;

void pinMode(uint8_t pin, uint8_t mode) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  _pinModes[pin] = mode;
  if (mode == INPUT_PULLUP) {
    _pinStates[pin] = HIGH; // Pullup = not pressed
  }
}

void digitalWrite(uint8_t pin, bool val) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  _pinStates[pin] = val;
}

int digitalRead(uint8_t pin) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  auto it = _pinStates.find(pin);
  return (it != _pinStates.end()) ? (it->second ? HIGH : LOW) : HIGH;
}

int analogRead(uint8_t pin) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  auto it = _analogValues.find(pin);
  return (it != _analogValues.end()) ? it->second : 2048;
}

void analogWrite(uint8_t pin, int value) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  printf("[SIM] analogWrite pin=%d value=%d\n", pin, value);
}

void analogReadResolution(int bits) {
  // No-op in simulation
}

void analogSetAttenuation(int atten) {
  // No-op in simulation
}

bool simGetPinState(uint8_t pin) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  auto it = _pinStates.find(pin);
  return (it != _pinStates.end()) ? it->second : HIGH;
}

void simSetPinState(uint8_t pin, bool state) {
  std::lock_guard<std::mutex> lock(_gpioMutex);
  _pinStates[pin] = state;
}

// ── LEDC Mock ───────────────────────────────────────────────
static std::map<uint8_t, double> _ledcFreq;
static std::map<uint8_t, uint8_t> _ledcDuty;

void ledcSetup(uint8_t channel, double freq, uint8_t resolution) {
  _ledcFreq[channel] = freq;
  _ledcDuty[channel] = 0;
  printf("[SIM] LEDC setup ch=%d freq=%.0f res=%d\n", channel, freq, resolution);
}

void ledcAttachPin(uint8_t pin, uint8_t channel) {
  printf("[SIM] LEDC attach pin=%d ch=%d\n", pin, channel);
}

void ledcWrite(uint8_t channel, uint8_t duty) {
  _ledcDuty[channel] = duty;
  if (duty > 0) {
    printf("[SIM] LEDC write ch=%d duty=%d\n", channel, duty);
  }
}

void ledcWriteTone(uint8_t channel, double freq) {
  _ledcFreq[channel] = freq;
  if (freq > 0) {
    printf("[SIM] LEDC tone ch=%d freq=%.0fHz\n", channel, freq);
  }
}

// ── FreeRTOS Stubs ─────────────────────────────────────────
struct MockMutex {
  bool locked = false;
  std::mutex mtx;
};

SemaphoreHandle_t xSemaphoreCreateMutex() {
  auto* m = new MockMutex();
  return (SemaphoreHandle_t)m;
}

int xSemaphoreTake(SemaphoreHandle_t sem, unsigned long timeout) {
  if (!sem) return pdFALSE;
  auto* m = (MockMutex*)sem;
  m->mtx.lock();
  m->locked = true;
  return pdTRUE;
}

int xSemaphoreGive(SemaphoreHandle_t sem) {
  if (!sem) return pdFALSE;
  auto* m = (MockMutex*)sem;
  m->locked = false;
  m->mtx.unlock();
  return pdTRUE;
}

// ── Time Stubs ─────────────────────────────────────────────
bool getLocalTime(struct tm* info, uint32_t timeout_ms) {
  if (!_simTimeSet) {
    // Initialize to a reasonable default
    _simTime.tm_year = 2026 - 1900;
    _simTime.tm_mon = 6;   // July
    _simTime.tm_mday = 28;
    _simTime.tm_hour = 12;
    _simTime.tm_min = 0;
    _simTime.tm_sec = 0;
    _simTime.tm_wday = 1;  // Monday
    _simTime.tm_isdst = 0;
    _simTimeSet = true;
  }

  // Advance simulated time
  time_t now_t = mktime(&_simTime);
  now_t += (timeout_ms / 1000);
  struct tm* result = localtime(&now_t);
  if (result) {
    _simTime = *result;
  }

  memcpy(info, &_simTime, sizeof(struct tm));
  return true;
}

void configTime(long gmtOffset, int daylightOffset, const char* server1, const char* server2) {
  printf("[SIM] configTime: GMT%+ld, server=%s\n", gmtOffset / 3600, server1);
}

void settimeofday(const struct timeval* tv, const void* tz) {
  if (tv) {
    time_t t = tv->tv_sec;
    _simTime = *localtime(&t);
    _simTimeSet = true;
    printf("[SIM] settimeofday: %ld\n", tv->tv_sec);
  }
}

void simSetTime(int year, int month, int day, int hour, int minute, int second) {
  _simTime.tm_year = year - 1900;
  _simTime.tm_mon = month - 1;
  _simTime.tm_mday = day;
  _simTime.tm_hour = hour;
  _simTime.tm_min = minute;
  _simTime.tm_sec = second;
  _simTime.tm_wday = 0; // Will be computed by mktime
  _simTime.tm_isdst = -1;
  mktime(&_simTime); // Normalize
  _simTimeSet = true;
  printf("[SIM] Time set: %04d-%02d-%02d %02d:%02d:%02d\n",
         year, month, day, hour, minute, second);
}

void simAdvanceTime(unsigned long ms) {
  _simMillis += ms;
  time_t now_t = mktime(&_simTime);
  now_t += (ms / 1000);
  struct tm* result = localtime(&now_t);
  if (result) {
    _simTime = *result;
  }
}

#endif // SIMULATION
