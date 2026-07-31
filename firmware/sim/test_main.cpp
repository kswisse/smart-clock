// ──────────────────────────────────────────────────────────────
// PIFKID 2026 — Functional Verification Test Runner
// ──────────────────────────────────────────────────────────────
// Runs after sim_main.cpp. Tests MVP business logic only.
// ──────────────────────────────────────────────────────────────

#ifdef SIMULATION

#include "test_common.h"
#include <cstdio>

extern void testBoot();
extern void testEventBus();
extern void testTodo();
extern void testAlarm();
extern void testSchedule();
extern void testTime();
extern void testWifi();
extern void testApiFlow();

int runTests() {
  printf("\n");
  printf("╔══════════════════════════════════════════════════════╗\n");
  printf("║  PIFKID 2026 — Functional Verification Tests        ║\n");
  printf("╚══════════════════════════════════════════════════════╝\n");

  testBoot();
  testEventBus();
  testTodo();
  testAlarm();
  testSchedule();
  testTime();
  testWifi();
  testApiFlow();

  printf("\n");
  printf("══════════════════════════════════════════════════════\n");
  printf("All test suites completed.\n");
  printf("══════════════════════════════════════════════════════\n");

  return 0;
}

#endif
