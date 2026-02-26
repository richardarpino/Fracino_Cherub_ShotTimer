#include <unity.h>
#include "Logic/ShotMonitor.h"
#include "Logic/ShotMonitor.cpp"
#include "Logic/ManualPumpTimer.h"
#include "Hardware/HardwareSwitch.h"
#include "Sensors/Virtual/DebouncedSwitch.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_shot_monitor_purging_filter() {
    MockRawSource mockPin;
    HardwareSwitch pumpHw(&mockPin, true);
    DebouncedSwitch pumpSw(&pumpHw, 150);
    ManualPumpTimer timer;
    SensorDispatcher registry;
    ShotMonitor monitor(&pumpSw, &timer, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH); // Pump OFF
    monitor.update();

    // 1. A short "Purge" shot (5 seconds)
    setMillis(1000);
    mockPin.setRawValue(LOW); // Pump ON
    monitor.update();
    
    setMillis(6000);
    mockPin.setRawValue(HIGH); // Pump OFF
    monitor.update();

    // LastValidShotTag should still be 0.0 because it was < 10s
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, registry.getLatest<LastValidShotTag>().value);
}

void test_shot_monitor_persistence() {
    MockRawSource mockPin;
    HardwareSwitch pumpHw(&mockPin, true);
    DebouncedSwitch pumpSw(&pumpHw, 150);
    ManualPumpTimer timer;
    SensorDispatcher registry;
    ShotMonitor monitor(&pumpSw, &timer, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH);
    monitor.update();

    // 1. A valid shot (25 seconds)
    setMillis(1000);
    mockPin.setRawValue(LOW);
    monitor.update();
    
    setMillis(26000);
    mockPin.setRawValue(HIGH);
    monitor.update();

    // Should be 25.0
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, registry.getLatest<LastValidShotTag>().value);

    // 2. A subsequent purge (5 seconds)
    setMillis(30000);
    mockPin.setRawValue(LOW);
    monitor.update();
    
    setMillis(35000);
    mockPin.setRawValue(HIGH);
    monitor.update();

    // Should STILL be 25.0 (Persistence of last good shot)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, registry.getLatest<LastValidShotTag>().value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_shot_monitor_purging_filter);
    RUN_TEST(test_shot_monitor_persistence);
    return UNITY_END();
}
