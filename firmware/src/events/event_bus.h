#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <Arduino.h>
#include <functional>
#include <vector>

#define EVENT_LOCK_TIMEOUT_MS 50
#define EVENT_QUEUE_MAX 64

enum EventType {
  EVT_NONE = 0,
  EVT_NAV_ENCODER,
  EVT_NAV_SELECT,
  EVT_NAV_BACK,
  EVT_SCREEN_CHANGED,
  EVT_TODO_CREATED,
  EVT_TODO_UPDATED,
  EVT_TODO_DELETED,
  EVT_TODO_TOGGLED,
  EVT_ALARM_CREATED,
  EVT_ALARM_UPDATED,
  EVT_ALARM_DELETED,
  EVT_ALARM_TRIGGERED,
  EVT_ALARM_STOPPED,
  EVT_SCHEDULE_CREATED,
  EVT_SCHEDULE_UPDATED,
  EVT_SCHEDULE_DELETED,
  EVT_TIME_CHANGED,
  EVT_WIFI_CONNECTED,
  EVT_WIFI_DISCONNECTED,
  EVT_DISPLAY_CHANGED,
  EVT_SOUND_CHANGED,
  EVT_SYSTEM_ERROR
};

struct Event {
  EventType type;
  uint16_t itemId;
  const char* message;
  unsigned long timestamp;

  Event() : type(EVT_NONE), itemId(0), message(""), timestamp(millis()) {}
  Event(EventType t, uint16_t id = 0, const char* msg = "")
    : type(t), itemId(id), message(msg), timestamp(millis()) {}
};

typedef std::function<void(const Event&)> EventCallback;

class EventBus {
public:
  void begin();
  void subscribe(EventType type, EventCallback callback);
  void unsubscribe(EventType type);
  void emit(const Event& event);
  void emit(EventType type, uint16_t itemId = 0, const char* message = "");
  void processQueue();
  size_t pendingCount();

private:
  struct Subscription {
    EventType type;
    EventCallback callback;
  };

  std::vector<Subscription> _subscriptions;
  std::vector<Event> _queue;
  bool _processing;
  SemaphoreHandle_t _queueMutex;
  SemaphoreHandle_t _subMutex;

  void _findSubscribers(EventType type, std::vector<Subscription>& result);
  void _lockQueue();
  void _unlockQueue();
  void _lockSubs();
  void _unlockSubs();
};

extern EventBus eventBus;

#endif // EVENT_BUS_H
