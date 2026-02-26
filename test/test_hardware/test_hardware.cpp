#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "Logic/ManualPumpTimer.h"
#include "Hardware/WeightSensor.h"
#include "Hardware/HardwareSwitch.h"
#include "Logic/ScaleLogic.h"
#include "Logic/ScaleLogic.cpp"
#include "Sensors/Virtual/DebouncedSwitch.h"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_pressure_calculation() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);
    mock.setRawValue(310);
    for(int i=0; i<100; i++) { setMillis(i); sensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 0.0f, sensor.getReading().value);

    mock.setRawValue(806);
    for(int i=100; i<200; i++) { setMillis(i); sensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1.0f, sensor.getReading().value);
}

void test_pressure_floor() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);
    mock.setRawValue(248);
    for(int i=0; i<100; i++) { setMillis(i); sensor.getReading(); }
    TEST_ASSERT_EQUAL_FLOAT(0.0f, sensor.getReading().value);
}

void test_weight_conversion() {
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    weightMock.setRawValue(5000); 
    for(int i=0; i<100; i++) { setMillis(i); weightSensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, weightSensor.getReading().value);
}

void test_shot_timer_logic() {
    MockRawSource mock;
    HardwareSwitch pumpHw(&mock, true);
    DebouncedSwitch pumpSw(&pumpHw, 150);
    ManualPumpTimer timer;
    ScaleLogic logic(&pumpSw, &timer, nullptr, nullptr, nullptr);
    
    setMillis(0);
    mock.setRawValue(HIGH); pumpSw.update(); logic.update();
    
    setMillis(1000);
    mock.setRawValue(LOW); pumpSw.update(); logic.update(); // Start
    
    setMillis(7000);
    mock.setRawValue(HIGH); pumpSw.update(); logic.update(); // Stop after 6s
    
    // In the simplified version, the 10s rule is GONE. 
    // So it should show 6.0s now.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 6.0f, timer.getReading().value);
    
    setMillis(8000);
    mock.setRawValue(LOW); pumpSw.update(); logic.update(); // Start again
    setMillis(20000); pumpSw.update(); logic.update(); 
    mock.setRawValue(HIGH); setMillis(20200); pumpSw.update(); logic.update(); // Stop after 12s
    
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 12.0f, timer.getReading().value);
}

void test_switch_logic() {
    MockRawSource mockPin;
    HardwareSwitch sw(&mockPin, true);
    setMillis(100);
    mockPin.setRawValue(HIGH); sw.update();
    TEST_ASSERT_FALSE(sw.isActive());
    
    setMillis(200);
    mockPin.setRawValue(LOW); sw.update();
    TEST_ASSERT_TRUE(sw.isActive());
    TEST_ASSERT_TRUE(sw.justStarted());
    
    setMillis(400);
    mockPin.setRawValue(HIGH); sw.update();
    TEST_ASSERT_FALSE(sw.isActive());
    TEST_ASSERT_TRUE(sw.justStopped());
}

void test_switch_idempotency() {
    MockRawSource mock;
    HardwareSwitch sw(&mock, true);
    setMillis(1000);
    mock.setRawValue(LOW);
    sw.update();
    TEST_ASSERT_TRUE(sw.justStarted());
    sw.update();
    TEST_ASSERT_TRUE(sw.justStarted());
    setMillis(1001);
    sw.update();
    TEST_ASSERT_FALSE(sw.justStarted());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_pressure_floor);
    RUN_TEST(test_weight_conversion);
    RUN_TEST(test_shot_timer_logic);
    RUN_TEST(test_switch_logic);
    RUN_TEST(test_switch_idempotency);
    return UNITY_END();
}
