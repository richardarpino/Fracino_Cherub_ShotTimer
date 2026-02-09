#ifndef SHOT_TIMER_H
#define SHOT_TIMER_H

#include <Arduino.h>
#include "../Interfaces/ISensor.h"

class ShotTimer : public ISensor {
public:
    ShotTimer(IRawSource* pumpSource, unsigned long debounceMs = 150, float minShotDuration = 10.0);

    // ISensor Implementation
    void update() override;
    Reading getReading() override;

private:
    enum TimerState {
        TIMER_READY,
        TIMER_RUNNING,
        TIMER_FINISHED
    };
    
    IRawSource* _pumpSource;
    unsigned long _debounceTime;
    unsigned long _startTime;
    unsigned long _lastActiveTime; // Last time the pump was seen active
    
    TimerState _state;
    float _finalTime;
    float _lastValidDuration;
    float _minShotDuration;
};

#endif
