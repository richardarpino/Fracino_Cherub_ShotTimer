#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "Hardware/WiFiSensor.h"
#include "Hardware/WiFiSensor.cpp"

void test_wifi_sensor_reporting() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor sensor;
    
    Reading r = sensor.getReading();
    TEST_ASSERT_EQUAL_STRING("CONNECTING...", r.label.c_str());
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.value);
    TEST_ASSERT_FALSE(r.isError);

    WiFi.setStatus(WL_CONNECTED);
    WiFi.setIP("192.168.1.50");
    r = sensor.getReading();
    TEST_ASSERT_EQUAL_STRING("CONNECTED\n192.168.1.50", r.label.c_str());
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r.value);
}

void test_wifi_sensor_error() {
    WiFiSensor sensor;
    WiFi.setStatus(WL_CONNECT_FAILED);
    
    Reading r = sensor.getReading();
    TEST_ASSERT_EQUAL_STRING("FAILED", r.label.c_str());
    TEST_ASSERT_TRUE(r.isError);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_sensor_reporting);
    RUN_TEST(test_wifi_sensor_error);
    return UNITY_END();
}
