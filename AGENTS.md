# Quy tắc làm việc

1. Schematic tại `../Smart_clock/hardware/kicad/Kicad-project/Kicad-project.kicad_sch`
   là nguồn sự thật cho pin và module. Không tự đổi pin theo code cũ.
2. Không thêm PCB workflow vào repo này.
3. `data/` chỉ chứa UI/UX; `firmware/` là bridge chạy trên ESP32-S3.
4. Chỉ có một firmware entry point: `firmware/main.cpp`.
5. Mọi cấu hình pin phải đi qua `firmware/src/core/pin_config.h`.
6. Không đưa verification report hoặc phase report được sinh tự động vào repo.
7. Trước khi ghép sang project chính, build firmware và upload LittleFS trên board thật.
8. Project chính dùng snake_case cho tên mới; giữ API JSON hiện hữu để UI không bị gãy.
