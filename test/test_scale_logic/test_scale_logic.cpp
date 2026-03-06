#include <unity.h>
#include "Logic/ScaleLogic.h"
#include "Logic/ScaleLogic.cpp"
#include "Sensors/Hardware/DigitalSensor.h"
#include "Logic/TaredWeight.h"
#include "Hardware/WeightSensor.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_scale_logic() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true, 0); // Active LOW, 0ms debounce for tests
    
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    TaredWeight taredWeight(&weightSensor);

    SensorDispatcher registry;
    registry.provide<PumpReading>(&pumpSensor);
    
    ScaleLogic logic(&taredWeight, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH); // Pump OFF
    weightMock.setRawValue(5000); // 5g on scale
    
    registry.update();
    logic.update();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, taredWeight.getReading().value);

    setMillis(1000);
    mockPin.setRawValue(LOW); // Pump ON
    registry.update();
    logic.update();

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, taredWeight.getReading().value);
    
    setMillis(2000);
    mockPin.setRawValue(HIGH); // Pump OFF
    registry.update();
    logic.update();
}

void test_scale_logic_edge_consumption() {
    MockRawSource mockPin;
    DigitalSensor pumpSensor(&mockPin, true, 0); // 0ms debounce for tests
    SensorDispatcher registry;
    registry.provide<PumpReading>(&pumpSensor);
    
    ScaleLogic logic(nullptr, &registry);

    setMillis(0);
    mockPin.setRawValue(HIGH);
    registry.update();
    logic.update();

    // 1. Pump starts
    setMillis(1000);
    mockPin.setRawValue(LOW);
    
    // In the new architecture, we call registry.update() centrally.
    registry.update();
    logic.update();

    // The logic successfully processed the frame. 
    // Further verification of edge persistence is in test_registry_switch.
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_scale_logic);
    RUN_TEST(test_scale_logic_edge_consumption);
    return UNITY_END();
}
