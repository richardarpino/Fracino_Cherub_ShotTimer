#include <unity.h>
#include "BoilerPressure.h"
#include "BoilerTemperature.h"
#include "../lib/ShotTimer/ShotTimer.cpp" 
#include "MockRawSource.h"
#include "stubs/Arduino.h"
#include "stubs/Arduino.cpp"

// --- Boiler Logic Tests ---

void test_pressure_calculation() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    // Test case 1: 0.5V (Sensor Min Voltage) -> 0.0 Bar
    // Math: Sensor 0.5V -> Divider 0.25V -> ADC: (0.25 / 3.3) * 4095 = 310
    mock.setRawValue(310);
    
    // Call multiple times to overcome EMA smoothing (alpha=0.1)
    for(int i=0; i<100; i++) {
        setMillis(i); // Advance time to allow sampling
        sensor.getReading();
    }
    
    Reading r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 0.0f, r.value);

    // Test case 2: 1.3V (approx 1.0 Bar)
    // Math: Sensor 1.3V -> Divider 0.65V -> ADC: (0.65 / 3.3) * 4095 = 806
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

    // Steady state at 1.0 Bar
    mock.setRawValue(806);
    for(int i=0; i<100; i++) {
        setMillis(i);
        pressureSensor.getReading();
    }
    
    Reading r = tempSensor.getReading();
    // 120.2C at 1.0 Bar gauge
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 120.2f, r.value);
}

// --- Shot Timer Tests ---

void test_shot_timer_logic() {
    MockRawSource mock;
    // 150ms debounce, 10s min duration
    ShotTimer timer(&mock, 150, 10.0);
    
    setMillis(0);
    mock.setRawValue(HIGH); // Pump OFF (Active Low)
    timer.getReading();
    
    // 1. Start shot
    setMillis(1000);
    mock.setRawValue(LOW); // Pump ON
    timer.getReading();
    
    // 2. Short duration (5s)
    setMillis(6000);
    mock.setRawValue(HIGH); // Pump OFF
    
    // We need to wait past debounce (150ms)
    setMillis(6200);
    Reading r = timer.getReading();
    
    // Should be "READY" or similar, value 0.0 because it's too short
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.value);
    
    // 3. Valid duration (12s)
    setMillis(7000);
    mock.setRawValue(LOW); // Pump ON again
    timer.getReading();
    
    setMillis(19000);
    // Must sample while ON to update _lastActiveTime
    timer.getReading(); 
    
    mock.setRawValue(HIGH); // Pump OFF
    setMillis(19200);
    r = timer.getReading();
    
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 12.0f, r.value);
    
    // 4. Persistence after a second short shot
    setMillis(25000);
    mock.setRawValue(LOW); timer.getReading();
    setMillis(27000); // 2s shot
    timer.getReading(); // Sample at 2s
    mock.setRawValue(HIGH); setMillis(27200);
    r = timer.getReading();
    
    // Should still show 12.0s
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 12.0f, r.value);
    TEST_ASSERT_EQUAL_STRING("READY", r.label.c_str());
}

// --- Hysteresis Tests ---

void test_hysteresis_logic() {
    MockRawSource mock;
    // Hysteresis threshold = 0.05 units
    // We'll use BoilerPressure (0.02 Bar threshold)
    BoilerPressure sensor(&mock);

    // 1. Establish steady state
    mock.setRawValue(806); // ~1.0 Bar
    for(int i=0; i<100; i++) {
        setMillis(i);
        sensor.getReading();
    }
    float baseline = sensor.getReading().value;
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, baseline);

    // 2. Small oscillation (below 0.02 Bar threshold)
    mock.setRawValue(812); 
    for(int i=100; i<200; i++) {
        setMillis(i);
        sensor.getReading();
    }
    // Should still show EXACTLY the baseline
    TEST_ASSERT_EQUAL_FLOAT(baseline, sensor.getReading().value);

    // 3. Large move (above threshold)
    // 1.05 Bar -> Sensor 1.34V -> Divider 0.67V -> ADC: 831
    mock.setRawValue(831);
    for(int i=200; i<300; i++) {
        setMillis(i);
        sensor.getReading();
    }
    // Should update now!
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.05f, sensor.getReading().value);
}

void test_pressure_floor() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    // Test a value below the 0.5V threshold (e.g. 0.4V)
    // 0.4V sensor -> 0.2V at divider -> ADC: (0.2 / 3.3) * 4095 = 248
    mock.setRawValue(248);
    for(int i=0; i<100; i++) {
        setMillis(i);
        sensor.getReading();
    }
    
    // Result should be exactly 0.0, not negative
    TEST_ASSERT_EQUAL_FLOAT(0.0f, sensor.getReading().value);
}

#ifdef NATIVE
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_shot_timer_logic);
    RUN_TEST(test_hysteresis_logic);
    RUN_TEST(test_pressure_floor);
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
