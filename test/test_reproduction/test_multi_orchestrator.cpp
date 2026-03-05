#include <unity.h>
#include "Logic/ShotMonitor.h"
#include "Logic/ShotMonitor.cpp"
#include "Logic/ScaleLogic.h"
#include "Logic/ScaleLogic.cpp"
#include "Logic/ManualPumpTimer.h"
#include "Sensors/Hardware/DigitalSensor.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

/**
 * REPRODUCTION: Multi-Orchestrator Edge Loss.
 * This test verifies that multiple components can observe the same edge
 * even if there is a delay between their updates, because the registry
 * remains stable between main loop passes.
 */
void test_reproduction_edge_loss_multi_orchestrator() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true, 0);
    ManualPumpTimer timer;
    SensorDispatcher registry;
    
    registry.provide<PumpTag>(&pumpSensor);
    
    // Two orchestrators watching the same pump
    ShotMonitor monitor(&timer, &registry);
    ScaleLogic scale(nullptr, &registry);

    // 1. Initial State (OFF)
    setMillis(0);
    mockPin.setRawValue(HIGH);
    registry.update();
    scale.update();
    monitor.update();

    // 2. Pump STARTS
    setMillis(1000);
    mockPin.setRawValue(LOW);
    
    // CENTRAL POLL: Happens once at frame start
    registry.update(); 
    
    // Orchestrator A reacts
    scale.update(); 
    
    // Even if there is a delay (different ms), Orchestrator B MUST see same edge
    setMillis(1001);
    monitor.update();

    // Verification: Monitor started the timer
    TEST_ASSERT_EQUAL_STRING("RUNNING", timer.getReading().label);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_reproduction_edge_loss_multi_orchestrator);
    return UNITY_END();
}
