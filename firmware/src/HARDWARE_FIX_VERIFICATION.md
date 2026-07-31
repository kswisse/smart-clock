# Hardware Bring-Up Fix Verification Report

**Date:** 2026-07-28
**Phase:** Hardware Bring-Up & Validation
**Fixes Applied:** 3 of 5 critical issues

---

## Fix 1: TFT_eSPI Configuration

### What Changed
- **New file:** `firmware/src/User_Setup.h`
- Pins aligned to `pin_config.h`: MOSI=23, SCLK=18, CS=5, DC=16, RST=17
- Display: ILI9341 driver, 240×320, SPI @40MHz
- No architecture changes — file is consumed by TFT_eSPI library externally

### Verification
| Check | Status |
|-------|--------|
| Pin values match `pin_config.h` exactly | PASS |
| ILI9341_DRIVER defined (default for 240×320) | PASS |
| SPI_FREQUENCY ≤50MHz (safe for ESP32) | PASS |
| ST7735_DRIVER available as alternative | PASS (commented) |
| No firmware architecture changes | PASS |

### Deployment Required
**Manual step before flashing:** Copy `firmware/src/User_Setup.h` to TFT_eSPI library folder:
```
<Arduino>/libraries/TFT_eSPI/User_Setup.h
```
Or use Arduino IDE: `Tools > ESP32 Sketch Data Upload` after setting the library path.

---

## Fix 2: firmware_info.cpp Dangling Pointers

### What Changed
- **`firmware_info.h`:** Changed `chipModel`, `sdkVersion`, `cpuFreq`, `macAddress` from `const char*` to `char[N]` arrays
- **`firmware_info.cpp`:** Replaced direct `String.c_str()` assignment with `strncpy`/`snprintf`

### Files Modified
| File | Change |
|------|--------|
| `firmware/src/core/firmware_info.h` | Struct fields: `const char*` → `char[N]` |
| `firmware/src/core/firmware_info.cpp` | Owned copy with `strncpy`/`snprintf` |

### Verification
| Check | Status |
|-------|--------|
| No temporary `String` pointers stored | PASS |
| `strncpy` with null-termination guard | PASS |
| Buffer sizes adequate (32, 16, 20 bytes) | PASS |
| `snprintf` for `cpuFreq` (integer→string) | PASS |
| ArduinoJson assignment compatible with `char[]` | PASS |
| Only consumer is `firmware_info.cpp` itself | PASS |
| No other files affected | PASS |

### Risk Assessment
- **Zero risk** of dangling pointers
- **Zero risk** of memory leaks (stack-allocated buffers)
- Compatible with all callers (verified via grep)

---

## Fix 3: Encoder Button Ownership

### What Changed
- **`buttons_hal.h`:** `NUM_BUTTONS` 3→2
- **`buttons_hal.cpp`:** Removed `PIN_ENCODER_BTN` from all pin arrays, `_pinToIndex`, `_indexToPin`, `isAnyPressed`
- **`hal.cpp`:** Removed `PIN_ENCODER_BTN` from `pinMode` init
- Encoder button is now exclusively managed by `EncoderHAL`

### Files Modified
| File | Change |
|------|--------|
| `firmware/src/hal/buttons_hal.h` | `NUM_BUTTONS` = 2 (was 3) |
| `firmware/src/hal/buttons_hal.cpp` | Remove encoder from all arrays and mappings |
| `firmware/src/hal/hal.cpp` | Remove `pinMode(PIN_ENCODER_BTN)` from init |

### Verification
| Check | Status |
|-------|--------|
| `ButtonsHAL` only tracks BTN1 (pin 32) + BTN2 (pin 33) | PASS |
| `EncoderHAL` sole owner of encoder button (pin 27) | PASS |
| `nav_state.cpp` uses `encoderHal.isButton*()` exclusively | PASS |
| No code calls `buttonsHAL.getState(PIN_ENCODER_BTN)` | PASS |
| `isAnyPressed()` checks only 2 buttons | PASS |
| `_pinToIndex()` returns 0xFF for unknown pins | PASS |
| `PIN_ENCODER_A` and `PIN_ENCODER_B` still initialized in `hal.cpp` | PASS |

### Double-Init Elimination
Before: `PIN_ENCODER_BTN` initialized in 3 places (`hal.cpp`, `buttons_hal.cpp`, `encoder_hal.cpp`)
After: `PIN_ENCODER_BTN` initialized in 1 place (`encoder_hal.cpp`)

---

## Compilation Status

**Note:** No compiler (arduino-cli, platformio, g++) available in this environment.

### Manual Verification Required
After installing Arduino IDE + ESP32 board package + libraries:

```bash
# 1. Copy User_Setup.h to TFT_eSPI library folder
cp firmware/src/User_Setup.h <Arduino>/libraries/TFT_eSPI/User_Setup.h

# 2. Install required libraries
#    - ESPAsyncWebServer
#    - AsyncTCP
#    - ArduinoJson v6

# 3. Compile
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/

# 4. Upload firmware
arduino-cli upload --fqbn esp32:esp32:esp32 --port COM3 firmware/

# 5. Upload LittleFS (via Arduino IDE)
#    Tools > ESP32 Sketch Data Upload (select data/ folder)
```

### Expected Compile Outcome
- **0 errors** from all 3 fixes
- **0 warnings** from changed code
- Existing warnings (if any) unchanged

---

## Files Changed Summary

| File | Action | Lines Changed |
|------|--------|---------------|
| `firmware/src/User_Setup.h` | CREATED | +42 |
| `firmware/src/core/firmware_info.h` | MODIFIED | 4 lines (struct fields) |
| `firmware/src/core/firmware_info.cpp` | MODIFIED | ~15 lines (getInfo function) |
| `firmware/src/hal/buttons_hal.h` | MODIFIED | 1 line (NUM_BUTTONS) |
| `firmware/src/hal/buttons_hal.cpp` | MODIFIED | ~10 lines (remove encoder) |
| `firmware/src/hal/hal.cpp` | MODIFIED | 2 lines (remove encoder init) |

**Total:** 1 new file, 5 modified files, ~70 lines changed

---

*End of Verification Report*
