#ifndef HAL_SPEAKER_H
#define HAL_SPEAKER_H

#include <Arduino.h>

// Alarm tone pattern
struct AlarmTone {
  uint16_t frequency;
  uint16_t durationMs;
  uint16_t pauseMs;
};

class SpeakerHAL {
public:
  void init();
  void update();           // Call from loop() for non-blocking playback

  void beep(uint16_t freq, uint16_t durationMs);
  void playTone(uint16_t freq, uint16_t durationMs, uint8_t volume);
  void alarm(uint8_t pattern);  // 0=default, 1=gentle, 2=urgent
  void stop();
  void snooze(uint32_t durationMs);

  void setVolume(uint8_t percent);
  uint8_t getVolume();
  bool isPlaying();
  bool isSnoozing();

private:
  bool _playing;
  bool _snoozing;
  uint8_t _volume;       // 0-100
  uint8_t _dutyCycle;    // 0-255 (derived from volume)

  // Playback state (non-blocking)
  bool _playbackActive;
  const AlarmTone* _currentPattern;
  uint8_t _patternLength;
  uint8_t _patternIndex;
  unsigned long _toneStartTime;
  bool _inTonePhase;

  // Snooze
  unsigned long _snoozeEndTime;

  void _startTone(uint16_t freq, uint8_t duty);
  void _stopTone();
  uint8_t _volumeToDuty(uint8_t vol);
};

extern SpeakerHAL speakerHAL;

#endif // HAL_SPEAKER_H
