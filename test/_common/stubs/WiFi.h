#ifndef WIFI_STUB_H
#define WIFI_STUB_H

#include <cstdint>
#include "Arduino.h"

#define WIFI_STA 1
void delay(uint32_t ms);

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

    // Logic verification
    void begin(const char* ssid, const char* pass) {
        _beginCalled = true;
        _ssid = ssid;
        _pass = pass;
    }
    void mode(int m) { _mode = m; }
    void disconnect(bool b) { _disconnectCalled = b; }

    bool wasBeginCalled() { return _beginCalled; }
    const char* getSsid() { return _ssid; }
    const char* getPass() { return _pass; }

private:
    wl_status_t _status = WL_DISCONNECTED;
    const char* _ip = "0.0.0.0";
    bool _beginCalled = false;
    const char* _ssid = nullptr;
    const char* _pass = nullptr;
    int _mode = 0;
    bool _disconnectCalled = false;
};

extern WiFiClass WiFi;

#endif
