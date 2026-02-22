#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "Hardware/WiFiSensor.h"
#include "Hardware/WiFiSensor.cpp"

void test_wifi_sensor_reporting() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor sensor("dummy", "dummy");
    
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
    WiFiSensor sensor("dummy", "dummy");
    WiFi.setStatus(WL_CONNECT_FAILED);
    
    Reading r = sensor.getReading();
    TEST_ASSERT_EQUAL_STRING("FAILED", r.label.c_str());
    TEST_ASSERT_TRUE(r.isError);
}

void test_wifi_sensor_construction_starts_wifi() {
    WiFiSensor sensor("TestSSID", "TestPass");

    TEST_ASSERT_TRUE(WiFi.wasBeginCalled());
    TEST_ASSERT_EQUAL_STRING("TestSSID", WiFi.getSsid());
    TEST_ASSERT_EQUAL_STRING("TestPass", WiFi.getPass());
}

void test_wifi_switch_behavior() {
    WiFiSensor sensor("dummy", "dummy");
    ISwitch* sw = (ISwitch*)&sensor;

    WiFi.setStatus(WL_DISCONNECTED);
    sw->update();
    TEST_ASSERT_FALSE(sw->isActive());

    WiFi.setStatus(WL_CONNECTED);
    sw->update();
    TEST_ASSERT_TRUE(sw->isActive());
    TEST_ASSERT_TRUE(sw->justStarted());

    sw->update();
    TEST_ASSERT_FALSE(sw->justStarted()); // Latch should clear
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_sensor_reporting);
    RUN_TEST(test_wifi_sensor_error);
    RUN_TEST(test_wifi_sensor_construction_starts_wifi);
    RUN_TEST(test_wifi_switch_behavior);
    return UNITY_END();
}
