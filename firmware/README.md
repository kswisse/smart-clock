# Firmware

ESP32 firmware for the PIFKID 2026 Smart Desk Clock.

## Quick Start

```bash
# Install PlatformIO
pip install platformio

# Compile for ESP32
pio run -e esp32

# Upload firmware
pio run -e esp32 --target upload

# Upload web UI (LittleFS)
pio run -e esp32 --target uploadfs

# Run simulation tests
pio run -e simulation
.pio/build/simulation/program.exe
```

## Structure

| Directory | Purpose |
|-----------|---------|
| `src/` | Production firmware source |
| `sim/` | Desktop simulation harness |
| `sim/tests/` | Functional verification tests |
| `docs/` | Firmware documentation |

## Architecture

See [AGENTS.md](../AGENTS.md) for the complete architecture reference.

## Simulation

The desktop simulation validates firmware logic without hardware. Run:

```bash
pio run -e simulation
.pio/build/simulation/program.exe
```

This runs 179 tests across 8 test suites covering all core functionality.
