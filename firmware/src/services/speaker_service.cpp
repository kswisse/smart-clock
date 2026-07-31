#include "speaker_service.h"
#include "../hal/speaker_hal.h"
#include "../events/event_bus.h"
#include "../utils/logger.h"

SpeakerService speakerService;

void SpeakerService::begin() {
  _activeAlarmId = 0;
  _alarmActive = false;

  // Subscribe to alarm events
  eventBus.subscribe(EVT_ALARM_TRIGGERED, [](const Event& e) {
    speakerService.handleAlarmTrigger(e.itemId);
  });

  eventBus.subscribe(EVT_ALARM_STOPPED, [](const Event& e) {
    speakerService.handleStop();
  });

  // Subscribe to sound control events (snooze, stop, volume)
  eventBus.subscribe(EVT_SOUND_CHANGED, [](const Event& e) {
    if (e.message) {
      if (strcmp(e.message, "snooze") == 0) {
        speakerService.handleSnooze();
      } else if (strcmp(e.message, "stop") == 0) {
        speakerService.handleStop();
      } else if (strcmp(e.message, "volume") == 0) {
        // Volume change with itemId as percent
        speakerService.handleVolumeChange(e.itemId);
      }
    }
  });

  logger.info("SPK_SVC", "Speaker service init (EventBus-driven)");
}

void SpeakerService::update() {
  speakerHAL.update();
}

void SpeakerService::handleAlarmTrigger(uint16_t alarmId) {
  // Prevent retriggering the same alarm while already playing
  if (_alarmActive && _activeAlarmId == alarmId) {
    logger.debug("SPK_SVC", "Alarm %d already active, skipping", alarmId);
    return;
  }

  _activeAlarmId = alarmId;
  _alarmActive = true;
  speakerHAL.alarm(0);  // Default pattern
  logger.info("SPK_SVC", "Alarm %d triggered via EventBus", alarmId);
}

void SpeakerService::handleSnooze() {
  if (_alarmActive) {
    speakerHAL.snooze(300000);  // 5 minutes
    _alarmActive = false;
    logger.info("SPK_SVC", "Snoozed alarm %d", _activeAlarmId);
  }
}

void SpeakerService::handleStop() {
  if (_alarmActive || speakerHAL.isPlaying()) {
    speakerHAL.stop();
    _alarmActive = false;
    _activeAlarmId = 0;
    logger.info("SPK_SVC", "Alarm stopped via EventBus");
  }
}

void SpeakerService::handleVolumeChange(uint8_t volume) {
  speakerHAL.setVolume(volume);
  logger.debug("SPK_SVC", "Volume set to %d%%", volume);
}

uint16_t SpeakerService::getActiveAlarmId() {
  return _activeAlarmId;
}

bool SpeakerService::isAlarmActive() {
  return _alarmActive;
}
