#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "Hardware/HardwareSwitch.h"
#include "Logic/BoilerTemperature.h"
#include "Logic/TaredWeight.h"
#include "Sensors/Virtual/DebouncedSwitch.h"
#include "Hardware/WeightSensor.h"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_temperature_calculation() {
    MockRawSource mock;
    BoilerPressure pressureSensor(&mock);
    BoilerTemperature tempSensor(&pressureSensor);
    mock.setRawValue(806);
    for(int i=0; i<100; i++) { setMillis(i); pressureSensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 120.2f, tempSensor.getReading().value);
}

void test_tared_sensor() {
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    TaredWeight taredSensor(&weightSensor);
    setMillis(0);
    weightMock.setRawValue(10000); 
    for(int i=1; i<100; i++) { setMillis(i); taredSensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 10.0f, taredSensor.getReading().value);

    taredSensor.tare();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, taredSensor.getReading().value);

    weightMock.setRawValue(12000); 
    for(int i=1100; i<1200; i++) { setMillis(i); taredSensor.getReading(); }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 2.0f, taredSensor.getReading().value);
}

void test_debounced_switch_persistence() {
    setMillis(1500);
    MockRawSource mockPin;
    HardwareSwitch hwSw(&mockPin);
    DebouncedSwitch delSw(&hwSw, 1000);
    mockPin.setRawValue(HIGH);
    hwSw.update(); delSw.update();
    TEST_ASSERT_FALSE(delSw.isActive());
    
    setMillis(1600);
    mockPin.setRawValue(LOW);
    hwSw.update(); delSw.update();
    TEST_ASSERT_TRUE(delSw.isActive());
    
    setMillis(2000);
    mockPin.setRawValue(HIGH);
    hwSw.update(); delSw.update();
    TEST_ASSERT_FALSE(hwSw.isActive());
    TEST_ASSERT_TRUE(delSw.isActive()); 
    
    setMillis(3001);
    delSw.update();
    TEST_ASSERT_FALSE(delSw.isActive());
    TEST_ASSERT_TRUE(delSw.justStopped());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_tared_sensor);
    RUN_TEST(test_debounced_switch_persistence);
    return UNITY_END();
}
