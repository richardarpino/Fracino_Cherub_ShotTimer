#ifndef SHOT_TIMER_H
#define SHOT_TIMER_H

#include <Arduino.h>

enum TimerState {
    TIMER_READY,
    TIMER_RUNNING,
    TIMER_FINISHED
};

class ShotTimer {
public:
    ShotTimer(unsigned long debounceMs = 150, float minShotDuration = 10.0);

    // Call this loop frequently with the current state of the pump pin (ACTIVE/INACTIVE)
    void update(bool isPumpActive, unsigned long currentTime);

    TimerState getState() const;
    float getCurrentTime() const;
    float getFinalTime() const;
    bool isRunning() const;

private:
    unsigned long _debounceTime;
    unsigned long _startTime;
    unsigned long _lastActiveTime; // Last time the pump was seen active
    
    TimerState _state;
    float _finalTime;
    float _lastValidDuration;
    float _minShotDuration;
};

#endif
