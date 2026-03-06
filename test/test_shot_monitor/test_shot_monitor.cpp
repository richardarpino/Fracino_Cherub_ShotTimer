#include <unity.h>
#include "Logic/ShotMonitor.h"
#include "Logic/ShotMonitor.cpp"
#include "Logic/ManualPumpTimer.h"
#include "Sensors/Hardware/DigitalSensor.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_shot_monitor_purging_filter() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true); // Active LOW
    ManualPumpTimer timer;
    SensorDispatcher registry;
    
    // Wire the sensor to the registry
    registry.provide<PumpReading>(&pumpSensor);
    
    // Monitor now only needs the timer and registry
    ShotMonitor monitor(&timer, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH); // Pump OFF
    registry.update();
    monitor.update();

    setMillis(1000);
    mockPin.setRawValue(LOW); // Pump ON
    registry.update();
    monitor.update();
    
    setMillis(6000);
    mockPin.setRawValue(HIGH); // Pump OFF
    registry.update();
    monitor.update();

    // LastValidShotReading should still be 0.0 because it was < 10s
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, registry.getLatest<LastValidShotReading>().value);
}

void test_shot_monitor_persistence() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true, 0);
    ManualPumpTimer timer;
    SensorDispatcher registry;
    
    registry.provide<PumpReading>(&pumpSensor);
    ShotMonitor monitor(&timer, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH);
    registry.update();
    monitor.update();

    setMillis(1000);
    mockPin.setRawValue(LOW);
    registry.update();
    monitor.update();
    
    setMillis(26000);
    mockPin.setRawValue(HIGH);
    registry.update();
    monitor.update();

    // Should be 25.0
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, registry.getLatest<LastValidShotReading>().value);

    // 2. A subsequent purge (5 seconds)
    setMillis(30000);
    mockPin.setRawValue(LOW);
    registry.update();
    monitor.update();
    
    setMillis(35000);
    mockPin.setRawValue(HIGH);
    registry.update();
    monitor.update();

    // Should STILL be 25.0 (Persistence of last good shot)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, registry.getLatest<LastValidShotReading>().value);
}

void test_shot_monitor_current_time_publication() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true, 0);
    ManualPumpTimer timer;
    SensorDispatcher registry;
    
    registry.provide<PumpReading>(&pumpSensor);
    ShotMonitor monitor(&timer, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH);
    registry.update();
    monitor.update();

    setMillis(1000);
    mockPin.setRawValue(LOW); // Start shot
    registry.update();
    monitor.update();
    
    // Register should show ~0s (or slightly more depending on when timer started)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, registry.getLatest<ShotTimeReading>().value);

    setMillis(5000); // 4 seconds later
    registry.update();
    monitor.update();

    // Registry MUST show the incremented time
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 4.0f, registry.getLatest<ShotTimeReading>().value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_shot_monitor_purging_filter);
    RUN_TEST(test_shot_monitor_persistence);
    RUN_TEST(test_shot_monitor_current_time_publication);
    return UNITY_END();
}
