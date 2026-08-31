# Ghép software vào Smart_clock

## 1. Nguồn sự thật phần cứng

Pin được đọc từ `Smart_clock/hardware/kicad/Kicad-project/Kicad-project.kicad_sch`:

| Module | Signal | GPIO |
|---|---|---:|
| ILI9341 | CS / DC / RST | 10 / 14 / 15 |
| ILI9341 | MOSI / SCLK / MISO | 11 / 12 / 13 |
| ILI9341 | Backlight | 7 |
| DS3231 | SDA / SCL | 8 / 9 |
| Input | Potentiometer | 1 |
| Input | CONTROL / STOP | 4 / 5 |
| Output | Buzzer | 41 |

Không dùng pin encoder 25/26/27 hoặc pin ESP32 thường 5/16/17 của bản software cũ.

## 2. Trách nhiệm giữa hai phần

- Giữ `Smart_clock` làm project chính và nơi lưu schematic.
- Dùng `data/` của repo này làm UI/UX duy nhất.
- Dùng `firmware/src/handlers`, `server`, `repositories` và `services` làm bridge giữa UI và thiết bị.
- Giữ driver hiển thị, RTC và buzzer nào đã được bench-test tốt hơn, nhưng mọi driver phải dùng
  `core/pin_config.h`.

## 3. Trình tự ghép an toàn

1. Build và chạy repo này độc lập trên ESP32-S3.
2. Bench-test theo thứ tự: TFT → DS3231 → potentiometer → CONTROL → STOP → buzzer.
3. Upload LittleFS và kiểm tra `/api/status`, Todo, Alarm, Schedule.
4. Trong project chính, tạo một PlatformIO environment ESP32-S3 rồi chuyển từng module đã test,
   không copy cả hai entry point vào cùng lúc.
5. Thay lớp `Alarm` cũ của project chính bằng model/service của repo này hoặc đổi tên lớp cũ.
   Hai lớp cùng tên không được compile chung.
6. Chỉ sau khi REST API và UI chạy ổn mới xóa các module firmware cũ trong project chính.

## 4. Hợp đồng UI ↔ firmware

UI cần các endpoint chính: `/api/status`, `/api/time`, `/api/todo`, `/api/alarm`,
`/api/schedule`, `/api/display`, `/api/sound`, `/api/wifi`, `/api/device` và `/api/sync`.
Response dùng dạng `{ success, code, message, timestamp, data }`; `data` có thể là object hoặc array.

## 5. Tiêu chí hoàn tất

- Build `esp32_s3` không lỗi.
- TFT đúng chiều 320x240 và không tranh chấp SPI.
- Mất WiFi hoặc reboot vẫn lấy giờ từ DS3231.
- CONTROL điều hướng/bật AP; STOP tắt buzzer ngay.
- CRUD từ web tồn tại lại sau reboot.
