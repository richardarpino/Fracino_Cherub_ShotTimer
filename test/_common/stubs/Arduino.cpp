#include "Arduino.h"

static unsigned long _mock_millis = 0;

unsigned long millis() {
    return _mock_millis;
}

void setMillis(unsigned long ms) {
    _mock_millis = ms;
}

void addMillis(unsigned long ms) {
    _mock_millis += ms;
}

void pinMode(int pin, int mode) {}
int digitalRead(int pin) { return HIGH; }
int analogRead(int pin) { return 0; }
