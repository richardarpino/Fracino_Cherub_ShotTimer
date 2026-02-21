#ifndef WIFI_STUB_H
#define WIFI_STUB_H

#include <cstdint>
#include "Arduino.h"

enum wl_status_t {
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
};

class WiFiClass {
public:
    void setStatus(wl_status_t status) { _status = status; }
    void setIP(const char* ip) { _ip = ip; }
    wl_status_t status() { return _status; }

    struct IPAddressStub {
        const char* _addr;
        String toString() { return String(_addr); }
    };

    IPAddressStub localIP() { return {_ip}; }

private:
    wl_status_t _status = WL_DISCONNECTED;
    const char* _ip = "0.0.0.0";
};

extern WiFiClass WiFi;

#endif
