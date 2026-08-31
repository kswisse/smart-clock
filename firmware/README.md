# Firmware bridge

Firmware PlatformIO cho ESP32-S3, dùng pin từ schematic của project chính.

```bash
pio run -e esp32_s3
pio run -e esp32_s3 -t upload
pio run -e esp32_s3 -t uploadfs
pio device monitor -b 115200
```

`uploadfs` lấy dữ liệu trực tiếp từ thư mục `../data` nhờ `data_dir` trong `platformio.ini`.

Luồng khởi động: HAL → LittleFS → TFT/input → WiFi → DS3231/NTP → service → REST server.
WiFi AP mặc định tắt; giữ nút CONTROL 3 giây để bật. Nút STOP luôn tắt âm báo.
