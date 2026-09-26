#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// ── Status LED ──────────────────────────────────────────────
#define PIN_LED_STATUS        2

// ── Battery ADC ─────────────────────────────────────────────
#define PIN_BATTERY_ADC       34

// ── Buttons ─────────────────────────────────────────────────
#define PIN_BUTTON_1          32
#define PIN_BUTTON_2          33

// ── Rotary Encoder ──────────────────────────────────────────
#define PIN_ENCODER_A         25
#define PIN_ENCODER_B         26
#define PIN_ENCODER_BTN       27

// ── Speaker (LEDC PWM) ─────────────────────────────────────
#define PIN_SPEAKER           21

// ── TFT Display (SPI) ──────────────────────────────────────
#define PIN_TFT_MOSI          23
#define PIN_TFT_SCLK          18
#define PIN_TFT_CS            5
#define PIN_TFT_DC            16
#define PIN_TFT_RST           17
#define PIN_TFT_BL            4

// ── Display Dimensions ──────────────────────────────────────
#define TFT_WIDTH             240
#define TFT_HEIGHT            320

// ── Display Rotation ────────────────────────────────────────
#define TFT_ROTATION          1

// ── Color Depth ─────────────────────────────────────────────
#define TFT_COLOR_DEPTH       16

#endif // PIN_CONFIG_H
