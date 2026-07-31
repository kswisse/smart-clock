#ifndef TFT_ESPI_H_MOCK
#define TFT_ESPI_H_MOCK
#ifdef SIMULATION

#include <cstdint>

#define TFT_BLACK    0x0000
#define TFT_WHITE    0xFFFF
#define TFT_RED      0xF800
#define TFT_GREEN    0x07E0
#define TFT_BLUE     0x001F
#define TFT_YELLOW   0xFFE0
#define TFT_CYAN     0x07FF
#define TFT_MAGENTA  0xF81F

#define TL_DATUM 0
#define TFT_DISPOFF 0x28
#define TFT_SLPIN   0x10
#define TFT_DISPON  0x29
#define TFT_SLPOUT  0x11

class TFT_eSPI {
public:
  void init() {}
  void setRotation(uint8_t r) {}
  void fillScreen(uint16_t c) {}
  void setTextDatum(uint8_t d) {}
  void setTextColor(uint16_t fg, uint16_t bg) {}
  void setTextSize(uint8_t s) {}
  void setCursor(int16_t x, int16_t y) {}
  void print(const char* t) {}
  void print(int v) {}
  void print(float v) {}
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {}
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {}
  void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {}
  void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {}
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {}
  void writecommand(uint16_t cmd) {}
};

#endif // SIMULATION
#endif // TFT_ESPI_H_MOCK
