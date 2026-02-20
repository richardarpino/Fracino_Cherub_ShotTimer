#include <unity.h>
#include "Hardware/ShotTimer.h"
#include "Hardware/HardwareSwitch.h"
#include "Virtual/DebouncedSwitch.h"
#include "Virtual/TaredWeight.h"
#include "Hardware/WeightSensor.h"
#include "Logic/ScaleLogic.h"
#include "Logic/ScaleLogic.cpp"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_scale_logic() {
    MockRawSource mockPin;
    HardwareSwitch pumpHw(&mockPin, true); // Active LOW
    DebouncedSwitch pumpSw(&pumpHw, 150);
    
    ShotTimer timer(0.5); // 0.5s min duration for test
    
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    TaredWeight taredWeight(&weightSensor);

    ScaleLogic logic(&pumpSw, &timer, &taredWeight);

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
    logic.update();

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, taredWeight.getReading().value);
    
    setMillis(2000);
    mockPin.setRawValue(HIGH); // Pump OFF
    logic.update();

    TEST_ASSERT_TRUE(timer.getReading().value > 0.9f); 
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_scale_logic);
    return UNITY_END();
}
