# Web UI

SPA không cần bundler, được nạp nguyên thư mục vào LittleFS.

- `js/core/`: bootstrap, router, state và config.
- `js/api/`: adapter REST API/demo.
- `js/pages/`: các màn hình.
- `js/components/`: UI dùng lại.

Khi host là `192.168.4.1` hoặc `pifkid.local`, UI gọi API trên ESP32. Trên localhost/Vercel,
UI dùng dữ liệu demo trong `localStorage`. Có thể ép mode bằng `?mode=device` hoặc `?mode=demo`.
