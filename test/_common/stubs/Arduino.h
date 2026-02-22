#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#include <stdint.h>

typedef uint8_t byte;

#ifdef __cplusplus
extern "C" {
#endif

unsigned long millis();
void setMillis(unsigned long ms);
void addMillis(unsigned long ms);

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
    const char* c_str() const { return std::string::c_str(); }
};
#endif

#endif
