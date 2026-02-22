#ifndef SHOT_TIMER_H
#define SHOT_TIMER_H

#include <Arduino.h>
#include "../../Interfaces/ISensor.h"
#include "../../Interfaces/ISwitch.h"

class ShotTimer : public ISensor {
public:
    ShotTimer(float minShotDuration = 10.0);

    // ISensor Implementation
    Reading getReading() override;
    SensorMetadata getMetadata() override;

    // External Control
    void start();
    void stop();

private:
    enum TimerState {
        TIMER_READY,
        TIMER_RUNNING,
        TIMER_FINISHED
    };
    
    unsigned long _startTime;
    
    TimerState _state;
    float _finalTime;
    float _lastValidDuration;
    float _minShotDuration;
};

#endif
