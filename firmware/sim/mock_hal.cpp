#ifdef SIMULATION

// ──────────────────────────────────────────────────────────────
// Mock HAL — Desktop Simulation
// ──────────────────────────────────────────────────────────────
// Replaces all real HAL implementations when SIMULATION is defined.
// Production code is completely unaffected.
// ──────────────────────────────────────────────────────────────

#include "mock_hal.h"

// ════════════════════════════════════════════════════════════════
// Global Mock Instances (replace production globals)
// ════════════════════════════════════════════════════════════════

MockDisplayHAL displayHAL;
MockButtonsHAL buttonsHAL;
MockEncoderHAL encoderHal;
MockTftStatusBar tftStatusBar;
MockTftClock tftClock;
MockTftTodo tftTodo;
MockTftSchedule tftSchedule;
MockTftManager tftManager;
MockNavState navState;

#endif // SIMULATION
