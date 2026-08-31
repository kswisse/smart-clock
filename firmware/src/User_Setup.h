// ──────────────────────────────────────────────────────────────
// TFT_eSPI User_Setup.h — PIFKID 2026 Smart Desk Clock
// ──────────────────────────────────────────────────────────────
// PlatformIO loads these values through platformio.ini.
// This file is kept as the human-readable hardware reference.
// ──────────────────────────────────────────────────────────────

#ifndef USER_SETUP_H
#define USER_SETUP_H

// ── Driver ──────────────────────────────────────────────────
#define ILI9341_DRIVER

// ── SPI Pins ────────────────────────────────────────────────
#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_MISO  13
#define TFT_CS    10
#define TFT_DC    14
#define TFT_RST   15

// ── Backlight (not managed by TFT_eSPI, handled in firmware) ─
#define TFT_BL   7

// ── Display Dimensions ──────────────────────────────────────
#define TFT_WIDTH   240
#define TFT_HEIGHT  320

// ── SPI Frequency ───────────────────────────────────────────
#define SPI_FREQUENCY  40000000   // 40 MHz — safe for most displays
#define SPI_READ_FREQUENCY  20000000

// ── Colour Depth ────────────────────────────────────────────
#define RGB565_SWAP_BYTES  0

// ── Backlight Control ───────────────────────────────────────
// Not used — backlight is managed via analogWrite in firmware
#define TFT_BACKLIGHT_ON HIGH

#endif // USER_SETUP_H
