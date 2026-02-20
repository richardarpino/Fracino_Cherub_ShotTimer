# Fracino Cherub Shot Timer & Boiler Monitor

![Compile and Test Native](https://github.com/richardarpino/Fracino_Cherub_ShotTimer/actions/workflows/compile.yml/badge.svg)

Custom ESP32 firmware for the Fracino Cherub espresso machine, providing real-time boiler diagnostics and an automated shot timer using a modular **data-pull architecture**.

---

## 🚀 Quick Start (60 Seconds)

1. **Hardware**: Connect sensors according to `include/pins.h`.
2. **Environment**: Install [PlatformIO IDE](https://platformio.org/).
3. **Configuration**:
    - `cp include/secrets.h.example include/secrets.h` (Add WiFi)
    - `cp include/pins.h.example include/pins.h` (Verify pins)
4. **Development Workflow**:
    - **Test (Native)**: `pio test -e native` (Run logic tests instantly on your PC)
    - **Build (Hardware)**: `pio run -e lilygo-t-display`
    - **Wait/Flash**: `pio run -e lilygo-t-display -t upload`

---

## 🏗 Modular Architecture

The project decouples hardware polling from application logic using a **Pull Model**.

### 📡 Data Sources (`lib/Interfaces`)
Everything providing data implements `ISensor` or `ISwitch`.

- **`ISensor`**: Provides continuous measurements (Pressure, Temperature, Weight).
    - `BoilerPressure`: Raw ADC -> Bar (Filtered).
    - `BoilerTemperature`: Bar -> Celsius conversion.
- **`ISwitch`**: Provides binary state with edge detection (`justStarted`, `justStopped`).
    - `HardwareSwitch`: Polled GPIO handler.
    - `DebouncedSwitch`: Persistence layer for debounce and gap filtering.

### 🛡️ Sensor Stability
To handle noisy signals (especially ESP32 ADC):
1. **Oversampling**: `ADCRawSource` averages 64 samples to floor noise.
2. **`FilteredSensor` Base Class**: Provides centralized **EMA Smoothing** and **Display Hysteresis** (stops value jitter).

### 🎨 UI & Widgets (`lib/UI`)
Widgets implement `IWidget` and pull data from sensors independently.
- **Late-Parenting**: Widgets are created abstractly and adopted by the layout manager.
- **Auto-Layout**: Managed in `main.cpp`. Widgets occupy slots: Top-Left (0), Bottom-Left (1), Top-Right (2), Bottom-Right (3).

---

## 🧪 Developer Guide

### 1. The TDD Workflow
We use a **Red-Green-Refactor** cycle. Logic-heavy components (like state machines) should be developed and verified in the `native` test suite before flashing to hardware.

```bash
# Run all unit tests
pio test -e native
```
Stubs for `Arduino.h` and hardware pins are provided in `test/stubs`.

### 2. Adding a New Sensor
Inherit from `FilteredSensor` to get automatic smoothing and hysteresis:
```cpp
class MySensor : public FilteredSensor {
    MySensor() : FilteredSensor(0.1f, 0.05f) {} // alpha, hysteresis threshold
    Reading getReading() override {
        updateFilter(newValue);
        return Reading(getStableDisplayValue(), "UNIT", "LABEL");
    }
};
```

### 3. Using Logic Switches
Use `ISwitch` to handle discrete events without boilerplate state tracking:
```cpp
if (pumpSw.justStarted()) { /* Trigger once on edge */ }
if (pumpSw.isActive())    { /* True while active */ }
```

---

## 🔌 Hardware Pins

Compatible with **LilyGo T-Display (ESP32)**. See `include/pins.h` for full mapping.

| Pin | Function | Hardware Required |
| :--- | :--- | :--- |
| **GPIO 32** | Pressure Sensor | 10kΩ + 10kΩ Voltage Divider |
| **GPIO 25** | Pump Signal | AC Optocoupler Isolation |
| **GPIO 35** | Theme Button | Built-in (Right button) |
| **GPIO 4** | LCD Backlight | Built-in |

---
*Maintained by the [Antigravity](https://deepmind.google/) Agentic AI.*
