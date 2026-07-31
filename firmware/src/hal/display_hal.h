#ifndef HAL_DISPLAY_H
#define HAL_DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#ifndef SIMULATION
class DisplayHAL {
public:
  void init();
  void clear();
  void clearRect(int x, int y, int w, int h);

  // Drawing primitives
  void fillRect(int x, int y, int w, int h, uint16_t color);
  void drawRect(int x, int y, int w, int h, uint16_t color);
  void fillCircle(int x, int y, int r, uint16_t color);
  void drawCircle(int x, int y, int r, uint16_t color);
  void drawLine(int x0, int y0, int x1, int y1, uint16_t color);

  // Text
  void setTextColor(uint16_t fg, uint16_t bg = 0xFFFF);
  void setTextSize(uint8_t size);
  void setCursor(int x, int y);
  void print(const char* text);
  void print(int value);
  void printAt(int x, int y, const char* text, uint8_t size = 1,
               uint16_t fg = 0xFFFF, uint16_t bg = 0x0000);

  // Brightness
  void setBrightness(uint8_t percent);
  uint8_t getBrightness();

  // Lifecycle
  void update();
  void sleep();
  void wake();
  bool isInitialized();

  // Raw access (for advanced rendering)
  TFT_eSPI& getTft();

private:
  TFT_eSPI _tft;
  bool _initialized;
  uint8_t _brightness;
  uint8_t _targetBrightness;
};

extern DisplayHAL displayHAL;

#endif // SIMULATION
#endif // HAL_DISPLAY_H
