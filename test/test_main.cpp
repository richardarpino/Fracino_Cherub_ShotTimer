#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "Virtual/BoilerTemperature.h"
#include "Hardware/ShotTimer.h"
#include "Hardware/WeightSensor.h"
#include "Virtual/TaredWeight.h"
#include "Interfaces/ISwitch.h"
#include "Hardware/HardwareSwitch.h"
#include "Virtual/DebouncedSwitch.h"
#include "MockRawSource.h"
#include "stubs/Arduino.h"
#include "stubs/Arduino.cpp"

// --- Boiler Logic Tests ---

void test_pressure_calculation() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    mock.setRawValue(310);
    for(int i=0; i<100; i++) {
        setMillis(i);
        sensor.getReading();
    }
    Reading r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 0.0f, r.value);

    mock.setRawValue(806);
    for(int i=100; i<200; i++) {
        setMillis(i);
        sensor.getReading();
    }
    r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1.0f, r.value);
}

void test_temperature_calculation() {
    MockRawSource mock;
    BoilerPressure pressureSensor(&mock);
    BoilerTemperature tempSensor(&pressureSensor);

    mock.setRawValue(806);
    for(int i=0; i<100; i++) {
        setMillis(i);
        pressureSensor.getReading();
    }
    
    Reading r = tempSensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 120.2f, r.value);
}

// --- Shot Timer Tests ---

void test_shot_timer_logic() {
    MockRawSource mock;
    HardwareSwitch pumpHw(&mock, true);
    DebouncedSwitch pumpSw(&pumpHw, 150);
    ShotTimer timer(&pumpSw, 10.0);
    
    setMillis(0);
    mock.setRawValue(HIGH); // Pump OFF
    timer.getReading();
    
    // 1. Start shot
    setMillis(1000);
    mock.setRawValue(LOW); // Pump ON
    timer.getReading();
    
    // 2. Short duration (5s)
    setMillis(6000);
    mock.setRawValue(HIGH); // Pump OFF
    
    setMillis(6200);
    Reading r = timer.getReading();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.value);
    
    // 3. Valid duration (12s)
    setMillis(7000);
    mock.setRawValue(LOW); // Pump ON again
    timer.getReading();
    
    setMillis(19000);
    timer.getReading(); 
    
    mock.setRawValue(HIGH); // Pump OFF
    setMillis(19200);
    r = timer.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 12.0f, r.value);
}

// --- Hysteresis Tests ---

void test_hysteresis_logic() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    mock.setRawValue(806);
    for(int i=0; i<100; i++) {
        setMillis(i);
        sensor.getReading();
    }
    float baseline = sensor.getReading().value;

    mock.setRawValue(812); 
    for(int i=100; i<200; i++) {
        setMillis(i);
        sensor.getReading();
    }
    TEST_ASSERT_EQUAL_FLOAT(baseline, sensor.getReading().value);

    mock.setRawValue(831);
    for(int i=200; i<300; i++) {
        setMillis(i);
        sensor.getReading();
    }
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.05f, sensor.getReading().value);
}

void test_pressure_floor() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    mock.setRawValue(248);
    for(int i=0; i<100; i++) {
        setMillis(i);
        sensor.getReading();
    }
    TEST_ASSERT_EQUAL_FLOAT(0.0f, sensor.getReading().value);
}

void test_weight_conversion() {
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);

    weightMock.setRawValue(5000); 
    for(int i=0; i<100; i++) {
        setMillis(i);
        weightSensor.getReading();
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, weightSensor.getReading().value);
}

void test_tared_sensor() {
    MockRawSource weightMock;
    WeightSensor weightSensor(&weightMock, 0.001f);
    
    MockRawSource switchMock;
    HardwareSwitch triggerSw(&switchMock, true); // Active LOW
    TaredWeight taredSensor(&weightSensor, &triggerSw);

    setMillis(0);
    switchMock.setRawValue(HIGH); // Switch OFF
    
    // 1. Establish steady state at 10.0g
    weightMock.setRawValue(10000); 
    for(int i=1; i<100; i++) {
        setMillis(i);
        taredSensor.getReading();
    }
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 10.0f, taredSensor.getReading().value);

    // 2. Trigger auto-tare via switch
    setMillis(1000);
    switchMock.setRawValue(LOW); // Switch ON
    
    // First read triggers tare
    Reading r = taredSensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, r.value);

    // 3. Increase weight to 12.0g (+2.0g)
    weightMock.setRawValue(12000); 
    for(int i=1100; i<1200; i++) {
        setMillis(i);
        taredSensor.getReading();
    }
    r = taredSensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 2.0f, r.value);

    // 4. Manual tare cleanup
    weightMock.setRawValue(15000);
    for(int i=3000; i<3100; i++) {
        setMillis(i);
        taredSensor.getReading();
    }
    taredSensor.tare();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, taredSensor.getReading().value);
}

void test_switch_idempotency() {
    MockRawSource mock;
    HardwareSwitch sw(&mock, true);
    
    setMillis(1000);
    mock.setRawValue(LOW);
    
    sw.update();
    TEST_ASSERT_TRUE(sw.justStarted());
    
    sw.update(); // Second poll in same ms
    TEST_ASSERT_TRUE(sw.justStarted());
    
    setMillis(1001);
    sw.update();
    TEST_ASSERT_FALSE(sw.justStarted());
}

void test_switch_logic() {
    MockRawSource mockPin;
    HardwareSwitch sw(&mockPin, true);
    
    setMillis(100);
    mockPin.setRawValue(HIGH);
    sw.update();
    TEST_ASSERT_FALSE(sw.isActive());
    
    setMillis(200);
    mockPin.setRawValue(LOW);
    sw.update();
    TEST_ASSERT_TRUE(sw.isActive());
    TEST_ASSERT_TRUE(sw.justStarted());
    
    setMillis(300);
    sw.update();
    TEST_ASSERT_TRUE(sw.isActive());
    TEST_ASSERT_FALSE(sw.justStarted());
    
    setMillis(400);
    mockPin.setRawValue(HIGH);
    sw.update();
    TEST_ASSERT_FALSE(sw.isActive());
    TEST_ASSERT_TRUE(sw.justStopped());
}

void test_debounced_switch_persistence() {
    setMillis(1500);
    MockRawSource mockPin;
    HardwareSwitch hwSw(&mockPin);
    DebouncedSwitch delSw(&hwSw, 1000);
    
    mockPin.setRawValue(HIGH);
    hwSw.update();
    delSw.update();
    TEST_ASSERT_FALSE(delSw.isActive());
    
    setMillis(1600);
    mockPin.setRawValue(LOW);
    hwSw.update();
    delSw.update();
    TEST_ASSERT_TRUE(delSw.isActive());
    
    setMillis(2000);
    mockPin.setRawValue(HIGH);
    hwSw.update();
    delSw.update();
    TEST_ASSERT_FALSE(hwSw.isActive());
    TEST_ASSERT_TRUE(delSw.isActive()); 
    
    setMillis(3001);
    delSw.update();
    TEST_ASSERT_FALSE(delSw.isActive());
    TEST_ASSERT_TRUE(delSw.justStopped());
}

#ifdef NATIVE
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_shot_timer_logic);
    RUN_TEST(test_hysteresis_logic);
    RUN_TEST(test_pressure_floor);
    RUN_TEST(test_weight_conversion);
    RUN_TEST(test_tared_sensor);
    RUN_TEST(test_switch_logic);
    RUN_TEST(test_debounced_switch_persistence);
    RUN_TEST(test_switch_idempotency);
    return UNITY_END();
}
#else
void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_shot_timer_logic);
    RUN_TEST(test_hysteresis_logic);
    RUN_TEST(test_pressure_floor);
    UNITY_END();
}
void loop() {}
#endif
