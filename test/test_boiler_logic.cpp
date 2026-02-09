#include <unity.h>
#include "../lib/BoilerPressure/BoilerPressure.h"
#include "../lib/Sensors/BoilerTemperature.h"
#include "MockRawSource.h"

// Test pressure calculation logic
void test_pressure_calculation() {
    MockRawSource mock;
    BoilerPressure sensor(&mock);

    // Test case 1: 0.5V (Sensor Min Voltage)
    // ESP32 ADC: (0.5V / 4.653V) * 4095 ~= 440
    // Actually, let's reverse the math in BoilerPressure:
    // Voltage at pin = Sensor Voltage * (R2 / (R1 + R2))
    // Voltage at pin = 0.5 * (10000 / 14700) = 0.3401V
    // ADC value = (0.3401 / 3.3) * 4095 = 422
    mock.setRawValue(422); 
    float pressure = sensor.calculatePressureFromRaw(mock.read());
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 0.0f, pressure);

    // Test case 2: 1.3V (approx 1.0 Bar)
    // Sensor Voltage = 1.3V
    // Voltage at pin = 1.3 * (10000 / 14700) = 0.884V
    // ADC value = (0.884 / 3.3) * 4095 = 1097
    mock.setRawValue(1097);
    pressure = sensor.calculatePressureFromRaw(mock.read());
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 1.0f, pressure);

    // Test case 3: 4.5V (Sensor Max Voltage)
    // Voltage at pin = 4.5 * (10000 / 14700) = 3.061V
    // ADC value = (3.061 / 3.3) * 4095 = 3798
    mock.setRawValue(3798);
    pressure = sensor.calculatePressureFromRaw(mock.read());
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 5.0f, pressure);
}

// Test temperature calculation logic independently
void test_temperature_calculation() {
    MockRawSource mock;
    BoilerPressure pressureSensor(&mock);
    BoilerTemperature tempSensor(&pressureSensor);

    // Test case 1: 1.0 Bar -> ~120C
    // (A=8.14019, B=1810.94, C=244.485)
    // P_abs = 1.0 + 1.01325 = 2.01325 Bar
    // P_mmHg = 2.01325 * 750.062 = 1510.06
    // T = 1810.94 / (8.14019 - log10(1510.06)) - 244.485 = 120.2C
    mock.setRawValue(1097); // ADC for ~1.0 Bar
    pressureSensor.update();
    
    Reading r = tempSensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 120.2f, r.value);
    TEST_ASSERT_EQUAL_STRING("C", r.unit.c_str());
}

// Dummy setup/loop for Unity
void setup() {
    delay(2000); // Wait for serial
    UNITY_BEGIN();
    RUN_TEST(test_pressure_calculation);
    RUN_TEST(test_temperature_calculation);
    UNITY_END();
}

void loop() {
}
