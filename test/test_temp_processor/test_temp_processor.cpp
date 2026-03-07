#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Processors/BoilerTemperatureProcessor.h"
#include "../../lib/Logic/Processors/BoilerTemperatureProcessor.cpp"
#include "../_common/stubs/SensorStub.h"

void test_temp_derivation() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    BoilerTemperatureProcessor processor(&registry);

    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<BoilerTempReading>(&processor);

    // 1.0 Bar -> Expect ~120.2C (Saturation temp)
    pressureSensor.setReading(1.0f);
    registry.update();

    Reading temp = registry.getLatest<BoilerTempReading>();
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 120.2f, temp.value);
}

void test_temp_filtering() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    BoilerTemperatureProcessor processor(&registry);

    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<BoilerTempReading>(&processor);

    // Initial reading
    pressureSensor.setReading(1.0f);
    registry.update();
    float firstTemp = registry.getLatest<BoilerTempReading>().value;

    // Small jitter should be filtered (threshold is 0.5C in legacy)
    pressureSensor.setReading(1.01f); // Very small change
    registry.update();
    float secondTemp = registry.getLatest<BoilerTempReading>().value;

    TEST_ASSERT_EQUAL_FLOAT(firstTemp, secondTemp);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_temp_derivation);
    RUN_TEST(test_temp_filtering);
    return UNITY_END();
}
