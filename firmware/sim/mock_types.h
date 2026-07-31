// ──────────────────────────────────────────────────────────────
// Mock Type Declarations for sim_main.cpp
// ──────────────────────────────────────────────────────────────
// This header provides types that are guarded out in production
// headers (#ifndef SIMULATION) but needed by the simulation driver.
// ──────────────────────────────────────────────────────────────
#ifndef MOCK_TYPES_H
#define MOCK_TYPES_H

#ifdef SIMULATION

#include <cstdint>
#include "arduino_stubs.h"

// ── ButtonState (from buttons_hal.h, guarded in simulation) ──
struct ButtonState {
  bool pressed;
  bool justPressed;
  bool justReleased;
  bool longPressed;
  uint32_t pressDuration;
};

// ── ScreenType (from nav_state.h, guarded in simulation) ─────
enum ScreenType {
  SCREEN_CLOCK,
  SCREEN_TODO,
  SCREEN_ALARM,
  SCREEN_SCHEDULE,
  SCREEN_SETTINGS,
  SCREEN_COUNT
};

#endif // SIMULATION
#endif // MOCK_TYPES_H
