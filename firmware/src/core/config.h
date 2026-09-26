#ifndef CONFIG_H
#define CONFIG_H

#include "pin_config.h"

// ── Firmware ────────────────────────────────────────────────
#define FW_VERSION            "1.0.0"
#define API_VERSION           "v1"
#define FW_BUILD_TIME         __DATE__ " " __TIME__

// ── Logging ─────────────────────────────────────────────────
#ifndef LOG_LEVEL
  #define LOG_LEVEL LOG_INFO
#endif

// ── WiFi AP ─────────────────────────────────────────────────
#define WIFI_AP_SSID          "PIFKID-2026"
#define WIFI_AP_PASS          "12345678"
#define WIFI_AP_CHANNEL       1
#define WIFI_AP_MAX_CONN      4

// ── WiFi STA ────────────────────────────────────────────────
#define WIFI_CONNECT_TIMEOUT_MS   10000
#define WIFI_MAX_RETRY_COUNT      5
#define WIFI_RECONNECT_INITIAL_MS 2000
#define WIFI_RECONNECT_MAX_MS     30000
#define WIFI_RECONNECT_MULTIPLIER 2
#define WIFI_RECONNECT_JITTER_PCT 20
#define WIFI_SCAN_INTERVAL_MS     30000
#define WIFI_STATUS_CHECK_MS      5000

// ── WiFi AP Timeout ─────────────────────────────────────────
#define WIFI_AP_TIMEOUT_MS     300000   // 5 minutes inactivity → stop AP

// ── Server ──────────────────────────────────────────────────
#define SERVER_PORT            80

// ── Buttons ─────────────────────────────────────────────────
#define BTN_DEBOUNCE_MS        50
#define BTN_WIFI_TOGGLE_MS     3000     // Long press duration to toggle WiFi AP

// ── Navigation ──────────────────────────────────────────────
#define NAV_DEBOUNCE_MS        50
#define NAV_DOUBLE_CLICK_MS    300
#define NAV_LONG_PRESS_MS      800

// ── Time ───────────────────────────────────────────────────
#define TIME_UPDATE_INTERVAL_MS 1000

// ── Todo ───────────────────────────────────────────────────
#define MAX_TITLE_LEN          64

// ── Encoder ─────────────────────────────────────────────────
#define ENCODER_STEPS_PER_DET  4
#define ENCODER_ACCEL_THRESHOLD 200
#define ENCODER_ACCEL_MAX      10

// ── Display ─────────────────────────────────────────────────
#define DISPLAY_WIDTH          TFT_WIDTH
#define DISPLAY_HEIGHT         TFT_HEIGHT
#define STATUS_BAR_HEIGHT      24
#define CLOCK_AREA_Y           STATUS_BAR_HEIGHT
#define CLOCK_AREA_H           120
#define CONTENT_Y              (STATUS_BAR_HEIGHT + CLOCK_AREA_H)
#define CONTENT_H              (DISPLAY_HEIGHT - CONTENT_Y)

// ── ILI9341 Sleep Commands ──────────────────────────────────
#ifndef TFT_SLPIN
  #define TFT_SLPIN            0x10
#endif
#ifndef TFT_SLPOUT
  #define TFT_SLPOUT           0x11
#endif

// ── Brightness ──────────────────────────────────────────────
#define BRIGHTNESS_MIN         10
#define BRIGHTNESS_MAX         255
#define BRIGHTNESS_DEFAULT     180
#define BRIGHTNESS_DIM         30
#define BRIGHTNESS_STEP        10

// ── Speaker (LEDC) ─────────────────────────────────────────
#define SPEAKER_CHANNEL        0
#define SPEAKER_FREQUENCY      1000
#define SPEAKER_RESOLUTION     8
#define SPEAKER_DUTY_MAX       255
#define ALARM_FREQ_1           880
#define ALARM_FREQ_2           1100
#define ALARM_TONE_MS          200
#define ALARM_PAUSE_MS         200
#define ALARM_REPEAT           6

// ── Redraw Timing ───────────────────────────────────────────
#define REDRAW_INTERVAL_MS     100
#define STATUS_REDRAW_MS       5000
#define CLOCK_REDRAW_MS        1000

// ── JSON ────────────────────────────────────────────────────
#define JSON_DOC_STATUS        1024
#define JSON_DOC_LARGE         2048

// ── Battery ADC ──────────────────────────────────────────────
#define BATTERY_ADC_MAX        4095
#define BATTERY_DIVIDER_RATIO  2.0f
#define BATTERY_VOLTAGE_MAX    4.2f
#define BATTERY_VOLTAGE_MIN    3.0f

// ── NTP ─────────────────────────────────────────────────────
#define NTP_SERVER             "pool.ntp.org"
#define NTP_SERVER_2           "time.nist.gov"
#define GMT_OFFSET_SEC         0
#define DAYLIGHT_OFFSET_SEC    0
#define DAYLIGHT_OFFSET        DAYLIGHT_OFFSET_SEC

// ── Filesystem Paths ────────────────────────────────────────
#define PATH_INDEX             "/index.html"

// ── MIME Types ──────────────────────────────────────────────
#define MIME_HTML              "text/html"
#define MIME_CSS               "text/css"
#define MIME_JS                "application/javascript"
#define MIME_JSON              "application/json"
#define MIME_SVG               "image/svg+xml"
#define MIME_PNG               "image/png"
#define MIME_ICO               "image/x-icon"

#endif // CONFIG_H
