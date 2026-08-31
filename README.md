# Smart Clock software integration

Repo này chứa phần web UI và firmware bridge để ghép vào project chính `Smart_clock`.
Schematic của project chính là nguồn sự thật cho toàn bộ pin; repo này không quản lý PCB.

## Cấu trúc

- `data/`: SPA chạy từ LittleFS; có demo mode khi mở ngoài ESP32.
- `firmware/main.cpp`: một entry point duy nhất.
- `firmware/src/`: HAL, service, repository, REST API và TFT UI.
- `docs/integration.md`: pinout và trình tự ghép vào project chính.

## Phần cứng mục tiêu

- ESP32-S3 DevKitC-1
- ILI9341 320x240 qua SPI
- DS3231 qua I2C
- Potentiometer điều hướng, nút CONTROL, nút STOP, buzzer

## Build nhanh

```bash
cd firmware
pio run -e esp32_s3
pio run -e esp32_s3 -t upload
pio run -e esp32_s3 -t uploadfs
```

Web UI tự dùng demo mode trên localhost. Dùng `?mode=device` để ép gọi REST API thật.

Chưa copy trực tiếp code này vào project chính trước khi bench-test TFT, RTC và ba input. Xem
[`docs/integration.md`](docs/integration.md) để ghép theo từng bước và tránh xung đột lớp `Alarm` cũ.
