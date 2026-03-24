#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#include <stdint.h>

typedef uint8_t byte;

#ifdef __cplusplus
extern "C" {
#endif

unsigned long millis();
void setHardwareTime(unsigned long ms);
void addHardwareTime(unsigned long ms);
void delayMicroseconds(unsigned int us);
void noInterrupts();
void interrupts();
void pinMode(int pin, int mode);
int digitalRead(int pin);
void digitalWrite(int pin, int value);
int analogRead(int pin);

// Mocking interface
void setDigitalRead(int pin, int value);
void setAnalogRead(int pin, int value);
void resetArduinoStubs();

// Sequence mocking
void setDigitalReadSequence(int pin, const int* values, int count);

#ifdef __cplusplus
}
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x01
#define OUTPUT 0x02
#define INPUT_PULLUP 0x04

#ifdef __cplusplus
#include <string>
class String : public std::string {
public:
    String(const char* s = "") : std::string(s) {}
    String(const std::string& s) : std::string(s) {}
    String(int value) : std::string(std::to_string(value)) {}
    const char* c_str() const { return std::string::c_str(); }
};
#endif

#endif
