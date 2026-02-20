# Fracino Cherub Shot Timer & Boiler Monitor

![Compile and Test Native](https://github.com/richardarpino/Fracino_Cherub_ShotTimer/actions/workflows/compile.yml/badge.svg)

A modular ESP32 firmware for the Fracino Cherub espresso machine. It provides real-time boiler diagnostics and an automated shot timer using a **Coordinator-based architecture**.

---

## 🚀 Get Started in 60 Seconds

### 1. Requirements
- **Hardware**: ESP32 (LilyGo T-Display clones), generic pressure sensor, AC Optocoupler for pump signal.
- **Environment**: [PlatformIO IDE](https://platformio.org/).

### 2. Configuration
```bash
# Setup secrets and pin mappings
cp include/secrets.h.example include/secrets.h  # Add your WiFi creds
cp include/pins.h.example include/pins.h        # Define your pins
```

### 3. Workflow
- **Test (Native)**: `pio test -e native` (Check logic instantly without hardware).
- **Build/Upload**: `pio run -e lilygo-t-display -t upload`

---

## 🏗 Modular Architecture

The project is split into three distinct layers to make it maintainable and testable.

### 🔌 Layer 1: Data Sources (`lib/Interfaces`)
Everything that measures or detects implements `ISensor` or `ISwitch`.
- **Sensors**: Continuous values like `BoilerPressure` or `WeightSensor`.
- **Switches**: Binary states with edge detection (`HardwareSwitch`, `DebouncedSwitch`).
- **Decorators**: Virtual sensors like `TaredWeight` or `BoilerTemperature`.

### 🧠 Layer 2: Logic Modules (`lib/Logic`)
This is the **"Brain"** of the machine. It coordinates behavior between sensors.
- **`ScaleLogic`**: Listens to the `pumpSw` and commands the `ShotTimer` and `Scale` (e.g., auto-taring when the pump starts).
- **Separation of Concerns**: Sensors handle *measurement*; Logic handles *behavior*.

### 🎨 Layer 3: UI & Widgets (`lib/UI`)
The UI is built with **LVGL**.
- **Widgets**: `SensorWidget`, `StatusWidget`. They pull data from sensors independently.
- **Themes**: Support for dynamic themes (`CandyTheme`, `ChristmasTheme`).

---

## 🧪 Developer Guide

### 1. Adding a New Feature (TDD)
We follow a **Red-Green-Refactor** workflow using the `native` test suite.
1. Add a test in `test/test_main.cpp`.
2. Run `pio test -e native` (it should fail).
3. Implement the logic in `lib/`.
4. Run tests again (it should pass).

### 2. Implementing Logic
If you want to add a new cross-component behavior (e.g., "Beep when pressure hits 2 bar"):
1. Instantiate your components in `main.cpp`.
2. Create or update a Logic module in `lib/Logic`.
3. Dispatch commands to sensors/actuators inside the `Logic::update()` method.

### 3. Handling Noisy Hardware
- **`ADCRawSource`**: Averages 64 samples at the hardware level.
- **`FilteredSensor`**: A base class providing **EMA Smoothing** and **Display Hysteresis** to stop value "flicker".

---

## 🔌 Hardware Reference

| Pin Mapping | Function | Notes |
| :--- | :--- | :--- |
| **GPIO 32** | Pressure Sensor | Analog 0-3.3V (use voltage divider) |
| **GPIO 25** | Pump Signal | AC Optocoupler (Active LOW/HIGH configurable) |
| **GPIO 35** | Button | Right button on T-Display |
| **GPIO 4** | Backlight | PWM controllable |

---

*Maintained by the [Antigravity](https://deepmind.google/) Agentic AI.*
