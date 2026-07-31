#ifndef SPEAKER_SERVICE_H
#define SPEAKER_SERVICE_H

#include <Arduino.h>

class SpeakerService {
public:
  void begin();
  void update();
  void handleAlarmTrigger(uint16_t alarmId);
  void handleSnooze();
  void handleStop();
  void handleVolumeChange(uint8_t volume);

  // Status queries
  uint16_t getActiveAlarmId();
  bool isAlarmActive();

private:
  uint16_t _activeAlarmId;
  bool _alarmActive;
};

extern SpeakerService speakerService;

#endif // SPEAKER_SERVICE_H
