#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// Hardware source of truth:
// Smart_clock/hardware/kicad/Kicad-project/Kicad-project.kicad_sch

// ILI9341 TFT (SPI)
#define PIN_TFT_CS             10
#define PIN_TFT_DC             14
#define PIN_TFT_RST            15
#define PIN_TFT_MOSI           11
#define PIN_TFT_SCLK           12
#define PIN_TFT_MISO           13
#define PIN_TFT_BL              7

// DS3231 RTC (I2C)
#define PIN_RTC_SDA             8
#define PIN_RTC_SCL             9

// User input
#define PIN_POTENTIOMETER       1
#define PIN_CONTROL_BUTTON      4
#define PIN_STOP_BUTTON         5

// Compatibility names used by the existing service/HAL code.
#define PIN_BUTTON_1 PIN_CONTROL_BUTTON
#define PIN_BUTTON_2 PIN_STOP_BUTTON

// Output
#define PIN_SPEAKER            41

// No battery ADC signal is connected to the ESP32 in the current schematic.
// PIN_LED_STATUS and PIN_BATTERY_ADC stay undefined, so code cannot silently
// claim GPIOs that are absent from the hardware design.

#endif // PIN_CONFIG_H
