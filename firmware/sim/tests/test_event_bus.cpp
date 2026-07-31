#ifdef SIMULATION

#include "../test_common.h"

static int _sub1Count = 0;
static int _sub2Count = 0;
static EventType _lastSub1Type = EVT_NONE;
static EventType _lastSub2Type = EVT_NONE;
static uint16_t _lastSub1Id = 0;

static void resetCounters() {
  _sub1Count = 0;
  _sub2Count = 0;
  _lastSub1Type = EVT_NONE;
  _lastSub2Type = EVT_NONE;
  _lastSub1Id = 0;
}

void testEventBus() {
  testBeginSuite("2. EventBus");

  resetCounters();

  // Test: subscribe + emit + process delivers event
  eventBus.subscribe(EVT_TODO_CREATED, [](const Event& e) {
    _sub1Count++;
    _lastSub1Type = e.type;
    _lastSub1Id = e.itemId;
  });
  eventBus.emit(EVT_TODO_CREATED, 42, "test item");
  testAssert(eventBus.pendingCount() == 1, "Emit queues one event");
  eventBus.processQueue();
  testAssert(_sub1Count == 1, "Subscriber received event");
  testAssert(_lastSub1Type == EVT_TODO_CREATED, "Event type correct");
  testAssert(_lastSub1Id == 42, "Event itemId correct");

  // Test: multiple subscribers receive same event
  resetCounters();
  eventBus.subscribe(EVT_TODO_CREATED, [](const Event& e) {
    _sub2Count++;
    _lastSub2Type = e.type;
  });
  eventBus.emit(EVT_TODO_CREATED, 1);
  eventBus.processQueue();
  testAssert(_sub1Count == 1, "First subscriber got event (multi-sub)");
  testAssert(_sub2Count == 1, "Second subscriber got event (multi-sub)");

  // Test: unrelated events not delivered
  resetCounters();
  eventBus.emit(EVT_ALARM_TRIGGERED, 99);
  eventBus.processQueue();
  testAssert(_sub1Count == 0, "Subscriber did NOT get unrelated event type");

  // Test: unsubscribe stops delivery
  resetCounters();
  eventBus.unsubscribe(EVT_TODO_CREATED);
  eventBus.emit(EVT_TODO_CREATED, 2);
  eventBus.processQueue();
  testAssert(_sub1Count == 0, "Unsubscribed callback not called");

  // Test: queue overflow handled (emit > 64 without processing)
  resetCounters();
  eventBus.subscribe(EVT_NAV_ENCODER, [](const Event& e) { _sub1Count++; });
  for (int i = 0; i < EVENT_QUEUE_MAX + 10; i++) {
    eventBus.emit(EVT_NAV_ENCODER, i);
  }
  size_t pending = eventBus.pendingCount();
  testAssert(pending <= EVENT_QUEUE_MAX, "Queue capped at max capacity");
  eventBus.processQueue();

  // Test: Event struct fields
  Event evt(EVT_ALARM_TRIGGERED, 7, "ring");
  testAssert(evt.type == EVT_ALARM_TRIGGERED, "Event constructor sets type");
  testAssert(evt.itemId == 7, "Event constructor sets itemId");
  testAssert(strcmp(evt.message, "ring") == 0, "Event constructor sets message");
  testAssert(evt.timestamp > 0, "Event constructor sets timestamp");

  // Clean up
  eventBus.unsubscribe(EVT_NAV_ENCODER);
  testEndSuite();
}

#endif
