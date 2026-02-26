#include <unity.h>
#include "Logic/ScaleLogic.h"
#include "Logic/ScaleLogic.cpp"
#include "Logic/ManualPumpTimer.h"
#include "Logic/BoilerTemperature.h"
#include "Hardware/HardwareSwitch.h"
#include "Sensors/Virtual/DebouncedSwitch.h"
#include "Logic/TaredWeight.h"
#include "Hardware/WeightSensor.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_scale_logic() {
    MockRawSource mockPin;
    HardwareSwitch pumpHw(&mockPin, true); // Active LOW
    DebouncedSwitch pumpSw(&pumpHw, 150);
    
    ManualPumpTimer timer; 
    
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    TaredWeight taredWeight(&weightSensor);

    SensorDispatcher registry;
    ScaleLogic logic(&pumpSw, &taredWeight, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH); // Pump OFF
    weightMock.setRawValue(5000); // 5g on scale
    
    for(int i=1; i<100; i++) {
        setMillis(i);
        taredWeight.getReading();
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, taredWeight.getReading().value);

    setMillis(1000);
    mockPin.setRawValue(LOW); // Pump ON
    pumpSw.update();
    logic.update();

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, taredWeight.getReading().value);
    
    setMillis(2000);
    mockPin.setRawValue(HIGH); // Pump OFF
    pumpSw.update();
    logic.update();

    // Note: Timer verification moved to test_shot_monitor
}

void test_scale_logic_edge_consumption() {
    MockRawSource mockPin;
    HardwareSwitch pumpHw(&mockPin, true);
    DebouncedSwitch pumpSw(&pumpHw, 150);
    SensorDispatcher registry;
    ScaleLogic logic(&pumpSw, nullptr, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH);
    logic.update();

    // 1. Pump starts
    setMillis(1000);
    mockPin.setRawValue(LOW);
    
    // 2. ScaleLogic update will now internally call pumpSw.update()
    setMillis(1001); 
    logic.update();

    // Verify it just ran without crashing - timer verification moved to ShotMonitor
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_scale_logic);
    RUN_TEST(test_scale_logic_edge_consumption);
    return UNITY_END();
}
