# Phase 5 — Hardware Validation Checklist

## TFT Display

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| TFT-1 | Frame rate | Measure redraw interval in `tft_manager.update()` | ≥8 FPS (≤125ms per frame) | |
| TFT-2 | Full redraw latency | Time `displayHAL.clear()` + full screen render | ≤30ms | |
| TFT-3 | Partial redraw latency | Time status bar only update | ≤5ms | |
| TFT-4 | Flicker | Visual inspection during normal clock display | No visible flicker | |
| TFT-5 | Ghosting | Visual inspection after screen transition | No残留 image artifacts | |
| TFT-6 | Brightness range | Set brightness 0%, 50%, 100% | Smooth dimming, no flicker at low | |
| TFT-7 | Brightness persistence | Set brightness, reboot | Same brightness after reboot | |
| TFT-8 | Color accuracy | Display known colors (red, green, blue, white) | Colors match expected | |
| TFT-9 | Text readability | Display text at sizes 1, 2, 3, 4 | All sizes readable | |
| TFT-10 | Sleep/wake cycle | Call `sleep()` then `wake()` | Display turns off/on cleanly | |
| TFT-11 | Long uptime | Run 24 hours continuously | No display degradation | |

## Buttons

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| BTN-1 | Debounce | Press rapidly (10x/sec) | No ghost triggers | |
| BTN-2 | Long press detection | Hold >1000ms | Long press event fires once | |
| BTN-3 | Short press detection | Tap <500ms | Short press event fires once | |
| BTN-4 | Multi-button | Press BTN1 + BTN2 simultaneously | Both register independently | |
| BTN-5 | Press duration accuracy | Hold 2000ms, measure reported duration | 2000ms ±100ms | |
| BTN-6 | Release detection | Press and release | JustReleased fires exactly once | |
| BTN-7 | All-up state | No buttons pressed | `isAnyPressed()` returns false | |

## Rotary Encoder

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| ENC-1 | Basic rotation | Rotate 10 detents CW | Position = +10 | |
| ENC-2 | Direction detection | Rotate CW then CCW | Correct sign on delta | |
| ENC-3 | Fast rotation | Rotate at maximum speed | No missed steps | |
| ENC-4 | Acceleration | Rotate slowly then fast | Acceleration factor increases | |
| ENC-5 | Acceleration decay | Stop fast rotation, wait 200ms | Acceleration resets to 1 | |
| ENC-6 | Button press | Press encoder button | `isButtonJustPressed()` returns true once | |
| ENC-7 | Button debounce | Press rapidly | No double triggers | |
| ENC-8 | Press duration | Hold 1500ms | `getButtonPressDuration()` ≥1500 | |

## Speaker

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| SPK-1 | Alarm trigger latency | Emit EVT_ALARM_TRIGGERED, measure time to sound | ≤500ms | |
| SPK-2 | Default alarm pattern | Play pattern 0 | Two-tone alternating at 880/1100Hz | |
| SPK-3 | Gentle pattern | Play pattern 1 | Softer two-tone at 440/523Hz | |
| SPK-4 | Urgent pattern | Play pattern 2 | Three-tone at 1000/1200/800Hz | |
| SPK-5 | Stop | Play alarm, call `stop()` | Sound stops immediately | |
| SPK-6 | Snooze | Play alarm, call `snooze(300000)` | Sound stops, resumes after 5min | |
| SPK-7 | Volume range | Set volume 0%, 50%, 100% | Audible volume changes | |
| SPK-8 | Volume during playback | Change volume while alarm plays | Volume changes without restart | |
| SPK-9 | Pattern loop | Let alarm play for 60 seconds | Pattern loops continuously | |
| SPK-10 | Non-blocking | Play alarm, verify `loop()` still runs | No freeze, TFT still updates | |

## RTC / Time

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| RTC-1 | NTP sync | Boot with WiFi, wait 5s | `isNtpSynced()` returns true | |
| RTC-2 | NTP accuracy | Compare with reference clock | ≤2 seconds drift after 24h | |
| RTC-3 | Timezone persistence | Set timezone, reboot | Timezone restored from config | |
| RTC-4 | Reboot time recovery | Set time, reboot | Time restored from NTP or saved | |
| RTC-5 | Power loss recovery | Pull power, restore | Time recovers within 30s (NTP) | |
| RTC-6 | Manual time set | POST `/api/time` with manual mode | Time updates correctly | |
| RTC-7 | Mode switching | Switch NTP → manual → NTP | Each mode works correctly | |
| RTC-8 | Day of week | Check `tm_wday` across midnight | Day increments correctly | |

## LittleFS

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| FS-1 | File creation | Create todo, verify file exists | `/todos.json` created | |
| FS-2 | File read | Read back todo | Data matches what was written | |
| FS-3 | File update | Update todo, read back | Updated fields persist | |
| FS-4 | File delete | Delete todo, verify file | File updated or removed | |
| FS-5 | Corruption recovery | Write garbage to `/todos.json`, reboot | Firmware starts, creates new file | |
| FS-6 | Power failure during write | Cut power during save | Data recovers (previous good state) | |
| FS-7 | Full filesystem | Fill flash to capacity | Graceful error, no crash | |
| FS-8 | Multiple files | Create todos + alarms + schedule | All files independent | |
| FS-9 | Concurrent access | API + TFT read simultaneously | No corruption | |
| FS-10 | Format and remount | Call `LittleFS.format()`, remount | Clean filesystem | |

## WiFi

| # | Test | Method | Pass Criteria | Status |
|---|------|--------|---------------|--------|
| WIFI-1 | AP mode boot | Boot without saved WiFi | AP starts, SSID visible | |
| WIFI-2 | STA connection | Enter WiFi credentials | Connects within 10s | |
| WIFI-3 | Disconnect recovery | Disconnect router, reconnect | Auto-reconnects within 30s | |
| WIFI-4 | AP fallback | STA fails, reboot | Falls back to AP mode | |
| WIFI-5 | Scan | Call scan endpoint | Returns list of networks | |
| WIFI-6 | Multiple clients | Connect 3 devices to AP | All can access web UI | |
| WIFI-7 | Long uptime | Run 24h with WiFi | Connection stays stable | |
| WIFI-8 | Credentials persistence | Save WiFi, reboot | Auto-connects to saved network | |
| WIFI-9 | Wrong password | Enter wrong password | Graceful failure, falls back to AP | |
| WIFI-10 | Signal strength | Vary distance from router | RSSI updates in status bar | |

---

*End of Validation Checklist*
