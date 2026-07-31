// ──────────────────────────────────────────────────────────────
// TFT_eSPI User_Setup.h — PIFKID 2026 Smart Desk Clock
// ──────────────────────────────────────────────────────────────
// Copy this file to your TFT_eSPI library folder:
//   <Arduino>/libraries/TFT_eSPI/User_Setup.h
//
// Pin definitions match firmware/src/core/pin_config.h
// ──────────────────────────────────────────────────────────────

#ifndef USER_SETUP_H
#define USER_SETUP_H

// ── Driver ──────────────────────────────────────────────────
// Generic STM32F411 is used as a safe default for most
// 240x320 ILI9341/ST7735 displays on ESP32 DevKit boards.
// If your display uses a different controller, change this.
#define DRIVER_SETUP  1

// ── Display Type ────────────────────────────────────────────
#define ILI9341_DRIVER
// #define ST7735_DRIVER    // Uncomment if your display is ST7735

// ── SPI Pins ────────────────────────────────────────────────
// Matches pin_config.h: MOSI=23, SCLK=18, CS=5, DC=16, RST=17
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_CS    5
#define TFT_DC    16
#define TFT_RST   17

// ── Backlight (not managed by TFT_eSPI, handled in firmware) ─
// #define TFT_BL   4

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
