#include "event_bus.h"
#include "../utils/logger.h"

EventBus eventBus;

void EventBus::begin() {
  _queueMutex = xSemaphoreCreateMutex();
  _subMutex = xSemaphoreCreateMutex();
  _processing = false;
  _queue.reserve(EVENT_QUEUE_MAX);
  logger.info("EVENT", "EventBus initialized (queue=%d)", EVENT_QUEUE_MAX);
}

void EventBus::subscribe(EventType type, EventCallback callback) {
  _lockSubs();
  Subscription sub;
  sub.type = type;
  sub.callback = callback;
  _subscriptions.push_back(sub);
  _unlockSubs();
  logger.debug("EVENT", "Subscribed to event %d", type);
}

void EventBus::unsubscribe(EventType type) {
  _lockSubs();
  _subscriptions.erase(
    std::remove_if(_subscriptions.begin(), _subscriptions.end(),
      [type](const Subscription& s) { return s.type == type; }),
    _subscriptions.end()
  );
  _unlockSubs();
}

void EventBus::emit(const Event& event) {
  _lockQueue();
  if (_queue.size() >= EVENT_QUEUE_MAX) {
    logger.warn("EVENT", "Queue full, dropping event %d", event.type);
    _unlockQueue();
    return;
  }
  _queue.push_back(event);
  _unlockQueue();
  logger.debug("EVENT", "Queued event %d (item=%d)", event.type, event.itemId);
}

void EventBus::emit(EventType type, uint16_t itemId, const char* message) {
  Event event(type, itemId, message);
  emit(event);
}

void EventBus::processQueue() {
  if (_processing) return;

  _lockQueue();
  if (_queue.empty()) {
    _unlockQueue();
    return;
  }
  _processing = true;

  while (!_queue.empty()) {
    Event event = _queue.front();
    _queue.erase(_queue.begin());
    _unlockQueue();

    std::vector<Subscription> subs;
    _findSubscribers(event.type, subs);
    for (auto& sub : subs) {
      sub.callback(event);
    }

    _lockQueue();
  }

  _processing = false;
  _unlockQueue();
}

size_t EventBus::pendingCount() {
  _lockQueue();
  size_t count = _queue.size();
  _unlockQueue();
  return count;
}

void EventBus::_findSubscribers(EventType type, std::vector<Subscription>& result) {
  _lockSubs();
  for (auto& sub : _subscriptions) {
    if (sub.type == type) {
      result.push_back(sub);
    }
  }
  _unlockSubs();
}

void EventBus::_lockQueue() {
  xSemaphoreTake(_queueMutex, pdMS_TO_TICKS(EVENT_LOCK_TIMEOUT_MS));
}

void EventBus::_unlockQueue() {
  xSemaphoreGive(_queueMutex);
}

void EventBus::_lockSubs() {
  xSemaphoreTake(_subMutex, pdMS_TO_TICKS(EVENT_LOCK_TIMEOUT_MS));
}

void EventBus::_unlockSubs() {
  xSemaphoreGive(_subMutex);
}
