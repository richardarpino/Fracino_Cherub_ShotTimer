#ifndef SHOT_TIMER_H
#define SHOT_TIMER_H

#include <Arduino.h>
#include "../../Interfaces/ISensor.h"
#include "../../Interfaces/ISwitch.h"

class ShotTimer : public ISensor {
public:
    ShotTimer(ISwitch* pumpSwitch, float minShotDuration = 10.0);

    // ISensor Implementation
    Reading getReading() override;

private:
    enum TimerState {
        TIMER_READY,
        TIMER_RUNNING,
        TIMER_FINISHED
    };
    
    ISwitch* _pump;
    unsigned long _startTime;
    
    TimerState _state;
    float _finalTime;
    float _lastValidDuration;
    float _minShotDuration;
};

#endif
