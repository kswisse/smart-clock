#include "speaker_hal.h"
#include "../core/pin_config.h"
#include "../core/config.h"
#include "../utils/logger.h"

// Alarm patterns
static const AlarmTone ALARM_DEFAULT[] = {
  {ALARM_FREQ_1, ALARM_TONE_MS, ALARM_PAUSE_MS},
  {ALARM_FREQ_2, ALARM_TONE_MS, ALARM_PAUSE_MS}
};
static const AlarmTone ALARM_GENTLE[] = {
  {440, 400, 300},
  {523, 400, 300}
};
static const AlarmTone ALARM_URGENT[] = {
  {1000, 150, 100},
  {1200, 150, 100},
  {800, 150, 100}
};

SpeakerHAL speakerHAL;

void SpeakerHAL::init() {
  _playing = false;
  _snoozing = false;
  _volume = 50;
  _dutyCycle = _volumeToDuty(_volume);
  _playbackActive = false;
  _patternIndex = 0;
  _snoozeEndTime = 0;

  // Configure LEDC channel
  ledcSetup(SPEAKER_CHANNEL, SPEAKER_FREQUENCY, SPEAKER_RESOLUTION);
  ledcAttachPin(PIN_SPEAKER, SPEAKER_CHANNEL);
  ledcWrite(SPEAKER_CHANNEL, 0); // Start silent

  logger.info("SPEAKER", "LEDC init ch=%d pin=%d", SPEAKER_CHANNEL, PIN_SPEAKER);
}

void SpeakerHAL::update() {
  unsigned long now = millis();

  // Check snooze expiry
  if (_snoozing && now >= _snoozeEndTime) {
    _snoozing = false;
    logger.info("SPEAKER", "Snooze expired, resuming alarm");
    alarm(0); // Resume default alarm
  }

  // Non-blocking pattern playback
  if (!_playbackActive || !_currentPattern) return;

  AlarmTone tone = _currentPattern[_patternIndex];
  unsigned long elapsed = now - _toneStartTime;

  if (_inTonePhase) {
    if (elapsed >= tone.durationMs) {
      // Switch to pause phase
      _stopTone();
      _inTonePhase = false;
      _toneStartTime = now;
    }
  } else {
    if (elapsed >= tone.pauseMs) {
      // Move to next tone
      _patternIndex++;
      if (_patternIndex >= _patternLength) {
        _patternIndex = 0;  // Loop
      }
      tone = _currentPattern[_patternIndex];
      _startTone(tone.frequency, _dutyCycle);
      _inTonePhase = true;
      _toneStartTime = now;
    }
  }
}

void SpeakerHAL::beep(uint16_t freq, uint16_t durationMs) {
  playTone(freq, durationMs, _volume);
}

void SpeakerHAL::playTone(uint16_t freq, uint16_t durationMs, uint8_t volume) {
  uint8_t duty = _volumeToDuty(volume);
  _startTone(freq, duty);
  _playbackActive = false;
  _playing = true;
  delay(durationMs);
  _stopTone();
  _playing = false;
}

void SpeakerHAL::alarm(uint8_t pattern) {
  if (_snoozing) return;

  switch (pattern) {
    case 1:
      _currentPattern = ALARM_GENTLE;
      _patternLength = sizeof(ALARM_GENTLE) / sizeof(AlarmTone);
      break;
    case 2:
      _currentPattern = ALARM_URGENT;
      _patternLength = sizeof(ALARM_URGENT) / sizeof(AlarmTone);
      break;
    default:
      _currentPattern = ALARM_DEFAULT;
      _patternLength = sizeof(ALARM_DEFAULT) / sizeof(AlarmTone);
      break;
  }

  _patternIndex = 0;
  _inTonePhase = true;
  _toneStartTime = millis();
  _playbackActive = true;
  _playing = true;

  // Start first tone immediately
  _startTone(_currentPattern[0].frequency, _dutyCycle);
  logger.info("SPEAKER", "Alarm pattern %d started", pattern);
}

void SpeakerHAL::stop() {
  _stopTone();
  _playbackActive = false;
  _playing = false;
  _currentPattern = nullptr;
  logger.debug("SPEAKER", "Stopped");
}

void SpeakerHAL::snooze(uint32_t durationMs) {
  _stopTone();
  _playbackActive = false;
  _snoozing = true;
  _snoozeEndTime = millis() + durationMs;
  logger.info("SPEAKER", "Snooze %dms", durationMs);
}

void SpeakerHAL::setVolume(uint8_t percent) {
  _volume = constrain(percent, 0, 100);
  _dutyCycle = _volumeToDuty(_volume);
  if (_playing) {
    ledcWrite(SPEAKER_CHANNEL, _dutyCycle);
  }
  logger.debug("SPEAKER", "Volume: %d%% (duty=%d)", _volume, _dutyCycle);
}

uint8_t SpeakerHAL::getVolume() {
  return _volume;
}

bool SpeakerHAL::isPlaying() {
  return _playing;
}

bool SpeakerHAL::isSnoozing() {
  return _snoozing;
}

void SpeakerHAL::_startTone(uint16_t freq, uint8_t duty) {
  ledcWriteTone(SPEAKER_CHANNEL, freq);
  ledcWrite(SPEAKER_CHANNEL, duty);
}

void SpeakerHAL::_stopTone() {
  ledcWrite(SPEAKER_CHANNEL, 0);
}

uint8_t SpeakerHAL::_volumeToDuty(uint8_t vol) {
  return map(vol, 0, 100, 0, SPEAKER_DUTY_MAX);
}
