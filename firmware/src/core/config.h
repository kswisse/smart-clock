#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

#include "pin_config.h"

// Firmware identity
#define FW_VERSION "1.0.0-integration"
#define API_VERSION "v1"
#define FW_BUILD_TIME __DATE__ " " __TIME__

// Serial/logging
#define LOG_LEVEL LOG_INFO

// Web server and LittleFS
#define SERVER_PORT 80
#define PATH_INDEX "/index.html"
#define MIME_HTML "text/html"
#define MIME_CSS "text/css"
#define MIME_JS "application/javascript"
#define MIME_SVG "image/svg+xml"
#define MIME_JSON "application/json"
#define MIME_PNG "image/png"
#define MIME_ICO "image/x-icon"

// JSON capacities. Keep the status document large enough for the three lists.
#define JSON_DOC_LARGE 8192
#define JSON_DOC_STATUS 12288
#define MAX_TITLE_LEN 64

// Display geometry: ILI9341 is physically 240x320 and used in landscape.
#define TFT_ROTATION 1
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define STATUS_BAR_HEIGHT 24
#define CONTENT_Y STATUS_BAR_HEIGHT
#define CONTENT_H (DISPLAY_HEIGHT - STATUS_BAR_HEIGHT)
#define CLOCK_AREA_Y CONTENT_Y
#define CLOCK_AREA_H CONTENT_H
#define REDRAW_INTERVAL_MS 100
#define STATUS_REDRAW_MS 1000
#define BRIGHTNESS_DEFAULT 80
#define BRIGHTNESS_MAX 255

// Input
#define BTN_DEBOUNCE_MS 50
#define NAV_LONG_PRESS_MS 1200
#define BTN_WIFI_TOGGLE_MS 3000
#define POT_LOGICAL_STEPS 20
#define POT_CHANGE_THRESHOLD 1
#define ENCODER_ACCEL_THRESHOLD 80
#define ENCODER_ACCEL_MAX 4

// Speaker
#define SPEAKER_CHANNEL 0
#define SPEAKER_FREQUENCY 2000
#define SPEAKER_RESOLUTION 8
#define SPEAKER_DUTY_MAX 127
#define ALARM_FREQ_1 880
#define ALARM_FREQ_2 1047
#define ALARM_TONE_MS 300
#define ALARM_PAUSE_MS 180

// Time: Vietnam (UTC+7), no daylight saving time.
#define NTP_SERVER "pool.ntp.org"
#define NTP_SERVER_2 "time.google.com"
#define GMT_OFFSET_SEC (7L * 3600L)
#define DAYLIGHT_OFFSET_SEC 0
#define DAYLIGHT_OFFSET 0
#define TIME_UPDATE_INTERVAL_MS 250

// WiFi AP is activated on demand by a 3-second control-button press.
#define WIFI_AP_SSID "PIFKID-2026"
#define WIFI_AP_PASS "12345678"
#define WIFI_AP_CHANNEL 1
#define WIFI_AP_MAX_CONN 4
#define WIFI_AP_TIMEOUT_MS (10UL * 60UL * 1000UL)
#define WIFI_SCAN_INTERVAL_MS 15000UL
#define WIFI_STATUS_CHECK_MS 1000UL
#define WIFI_CONNECT_TIMEOUT_MS 10000UL
#define WIFI_RECONNECT_INITIAL_MS 1000UL
#define WIFI_RECONNECT_MAX_MS 30000UL
#define WIFI_RECONNECT_MULTIPLIER 2UL
#define WIFI_RECONNECT_JITTER_PCT 20UL
#define WIFI_MAX_RETRY_COUNT 10

// Battery helpers remain available, but PIN_BATTERY_ADC is intentionally absent.
#define BATTERY_ADC_MAX 4095
#define BATTERY_DIVIDER_RATIO 2.0f
#define BATTERY_VOLTAGE_MIN 3.2f
#define BATTERY_VOLTAGE_MAX 4.2f

#endif // CORE_CONFIG_H
