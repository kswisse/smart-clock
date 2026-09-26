# CI Fix Verification — 2026-09-26

## Bug

The CI simulation job failed on every run (5/5 recent runs red).

## Root Causes

1. **`.gitignore` pattern `core/`** (intended for a root-level unrelated folder) also matched `firmware/src/core/`. `config.h`, `pin_config.h`, and `firmware_info.*` were never committed, so a fresh clone could not compile at all.
2. **Non-portable overloads** — `sim/arduino_stubs.h` (`String(size_t)` vs `String(unsigned long)`) and `sim/mock_headers/ArduinoJson.h` (`operator|(size_t)` vs `operator|(unsigned long)`) redeclare the same signature on LP64 Linux, where `size_t == unsigned long`. Compiles on Win64, fails on Linux.
3. **Mock lag** — the simulation mocks were missing APIs used by firmware code: `LittleFS.open(path)` default mode, `File::openNextFile()/name()/isDirectory()` directory iteration, `String::lastIndexOf(char)`, and `JsonDocument` was typedef'd to `DynamicJsonDocument`, so `StaticJsonDocument<512>` could not bind to `JsonDocument&`.

## Fix

- Root-anchored the unrelated-folder ignore patterns (`/core/`, `/experiments/`, `/repo/`, `/archive/`) and committed `firmware/src/core/*`.
- Guarded the size_t-only overloads with `#if SIZE_MAX != ULONG_MAX` (kept on Win64, skipped on LP64).
- Extended the sim mocks: directory iteration in `File`, `littlefs_impl::isDir`, `String::lastIndexOf`, `JsonDocument` = `StaticJsonDocumentBase` (shared base of both document types, mirroring real ArduinoJson).
- Removed the unreferenced root-level prototype sketch `pifkid_esp32.ino` (superseded by `firmware/main.ino`/`main.cpp`; referenced nowhere).

## Verification

| Check | Result |
|-------|--------|
| `pio run -e simulation` (MinGW g++ 16.1) | SUCCESS |
| `./.pio/build/simulation/program` | All test suites passed, exit 0 |
| CI `Check Formatting` job | Already passing (unchanged) |
| CI `Simulation Tests` job | Expected green on push (ubuntu-latest, g++) |
