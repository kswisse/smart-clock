#include "display_hal.h"

#ifndef SIMULATION
#include "../core/pin_config.h"
#include "../core/config.h"
#include "../utils/logger.h"

DisplayHAL displayHAL;

void DisplayHAL::init() {
  _tft = TFT_eSPI();
  _tft.init();
  _tft.setRotation(TFT_ROTATION);
  _tft.fillScreen(TFT_BLACK);
  _tft.setTextDatum(TL_DATUM);

  _brightness = BRIGHTNESS_DEFAULT;
  _targetBrightness = BRIGHTNESS_DEFAULT;
  setBrightness(_brightness);

  _initialized = true;
  logger.info("DISPLAY", "TFT init %dx%d, rot=%d", TFT_WIDTH, TFT_HEIGHT, TFT_ROTATION);
}

void DisplayHAL::clear() {
  _tft.fillScreen(TFT_BLACK);
}

void DisplayHAL::clearRect(int x, int y, int w, int h) {
  _tft.fillRect(x, y, w, h, TFT_BLACK);
}

void DisplayHAL::fillRect(int x, int y, int w, int h, uint16_t color) {
  _tft.fillRect(x, y, w, h, color);
}

void DisplayHAL::drawRect(int x, int y, int w, int h, uint16_t color) {
  _tft.drawRect(x, y, w, h, color);
}

void DisplayHAL::fillCircle(int x, int y, int r, uint16_t color) {
  _tft.fillCircle(x, y, r, color);
}

void DisplayHAL::drawCircle(int x, int y, int r, uint16_t color) {
  _tft.drawCircle(x, y, r, color);
}

void DisplayHAL::drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
  _tft.drawLine(x0, y0, x1, y1, color);
}

void DisplayHAL::setTextColor(uint16_t fg, uint16_t bg) {
  _tft.setTextColor(fg, bg);
}

void DisplayHAL::setTextSize(uint8_t size) {
  _tft.setTextSize(size);
}

void DisplayHAL::setCursor(int x, int y) {
  _tft.setCursor(x, y);
}

void DisplayHAL::print(const char* text) {
  _tft.print(text);
}

void DisplayHAL::print(int value) {
  _tft.print(value);
}

void DisplayHAL::printAt(int x, int y, const char* text, uint8_t size,
                         uint16_t fg, uint16_t bg) {
  _tft.setCursor(x, y);
  _tft.setTextSize(size);
  _tft.setTextColor(fg, bg);
  _tft.print(text);
}

void DisplayHAL::setBrightness(uint8_t percent) {
  _targetBrightness = map(constrain(percent, 0, 100), 0, 100, 0, BRIGHTNESS_MAX);
  if (_targetBrightness != _brightness) {
    _brightness = _targetBrightness;
    analogWrite(PIN_TFT_BL, _brightness);
    logger.debug("DISPLAY", "Brightness: %d%% (%d)", percent, _brightness);
  }
}

uint8_t DisplayHAL::getBrightness() {
  return map(_brightness, 0, BRIGHTNESS_MAX, 0, 100);
}

void DisplayHAL::update() {
  // TFT_eSPI pushes buffer on print/draw calls; no explicit flush needed
}

void DisplayHAL::sleep() {
  _tft.writecommand(TFT_DISPOFF);
  _tft.writecommand(TFT_SLPIN);
  logger.info("DISPLAY", "Sleep");
}

void DisplayHAL::wake() {
  _tft.writecommand(TFT_SLPOUT);
  _tft.writecommand(TFT_DISPON);
  logger.info("DISPLAY", "Wake");
}

bool DisplayHAL::isInitialized() {
  return _initialized;
}

TFT_eSPI& DisplayHAL::getTft() {
  return _tft;
}

#endif // SIMULATION
