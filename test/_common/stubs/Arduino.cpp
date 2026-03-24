#include "Arduino.h"
#include <string.h>

static unsigned long _mock_millis = 0;
static int _digital_pins[40];
static int _analog_pins[40];
static const int* _digital_sequences[40];
static int _digital_sequence_counts[40];
static int _digital_sequence_indices[40];

unsigned long millis() {
    return _mock_millis;
}

void setHardwareTime(unsigned long ms) {
    _mock_millis = ms;
}

void addHardwareTime(unsigned long ms) {
    _mock_millis += ms;
}
 
void noInterrupts() {}
void interrupts() {}
void delayMicroseconds(unsigned int us) {
    // We don't advance mock_millis here as it's too fine-grained
}

void pinMode(int pin, int mode) {}

void digitalWrite(int pin, int value) {
    if (pin >= 0 && pin < 40) _digital_pins[pin] = value;
}

int digitalRead(int pin) {
    if (pin < 0 || pin >= 40) return HIGH;
    
    // Check if there is a sequence active for this pin
    if (_digital_sequences[pin] != nullptr && _digital_sequence_indices[pin] < _digital_sequence_counts[pin]) {
        return _digital_sequences[pin][_digital_sequence_indices[pin]++];
    }
    
    return _digital_pins[pin];
}

int analogRead(int pin) {
    if (pin < 0 || pin >= 40) return 0;
    return _analog_pins[pin];
}

void setDigitalRead(int pin, int value) {
    if (pin >= 0 && pin < 40) {
        _digital_pins[pin] = value;
        _digital_sequences[pin] = nullptr; // Reset sequence if manual value set
    }
}

void setDigitalReadSequence(int pin, const int* values, int count) {
    if (pin >= 0 && pin < 40) {
        _digital_sequences[pin] = values;
        _digital_sequence_counts[pin] = count;
        _digital_sequence_indices[pin] = 0;
    }
}

void setAnalogRead(int pin, int value) {
    if (pin >= 0 && pin < 40) _analog_pins[pin] = value;
}

void resetArduinoStubs() {
    _mock_millis = 0;
    for (int i = 0; i < 40; i++) {
        _digital_pins[i] = HIGH;
        _analog_pins[i] = 0;
        _digital_sequences[i] = nullptr;
        _digital_sequence_counts[i] = 0;
        _digital_sequence_indices[i] = 0;
    }
}
