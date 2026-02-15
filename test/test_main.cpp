#include <unity.h>
#include "BoilerPressure.h"
#include "BoilerTemperature.h"
#include "ShotTimer.cpp" // Include implementation for native build
#include "MockRawSource.h"
#include "Arduino.h"
#include "Arduino.cpp"

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

#ifdef NATIVE
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_shot_timer_logic);
    return UNITY_END();
}
#else
void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    RUN_TEST(test_shot_timer_logic);
    UNITY_END();
}
void loop() {}
#endif
