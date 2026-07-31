#ifdef SIMULATION

// ──────────────────────────────────────────────────────────────
// Mock Services — Desktop Simulation
// ──────────────────────────────────────────────────────────────
// Replaces WiFi and Time services when SIMULATION is defined.
// ──────────────────────────────────────────────────────────────

#include "mock_services.h"

// ── Global Mock Service Instances ────────────────────────────
MockWifiService wifiService;
MockTimeService timeService;

#endif // SIMULATION
