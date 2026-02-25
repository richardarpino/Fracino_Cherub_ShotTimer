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

---

## 🏗 Modular Architecture

The project is split into three distinct layers to make it maintainable and testable.

### 🔌 Layer 1: Data Sources & Interfaces (`lib/Interfaces`)
Everything that measures or detects implements `ISensor` or `ISwitch`.
- **Sensors**: Continuous values like `BoilerPressure` or `WeightSensor`.
- **Switches**: Binary states with edge detection (`HardwareSwitch`, `DebouncedSwitch`).
- **Decorators**: Virtual sensors like `TaredWeight` or `BoilerTemperature`.

### 🧠 Layer 2: Logic Modules (`lib/Logic`)
This is the **"Brain"** of the machine. It coordinates behavior between sensors using **Orchestrators**.
- **`ScaleLogic`**: Coordinates the shot timer and scale auto-taring.
- **`StartupLogic`**: Manages the boot sequence (WiFi/OTA).
- **Polling**: Orchestrators are responsible for polling their required switches/sensors.

### 🏭 Layer 3: Providers & Factories (`lib/Factories`)
We use a **Pure Factory** pattern to manage hardware.
- **`MachineFactory`**: The single point of instantiation for all hardware.
- **`IMachineProvider`**: Segregated interfaces (`ISensorProvider`, `ISwitchProvider`, `IThemeProvider`) used by logic modules to access hardware without knowing implementation details.
- **Lazy Initialization**: Hardware services (like WiFi/OTA) are instantiated only when first requested. Instantiation *is* the activation signal.

---

## 🎨 UI Gallery & Automated Documentation

The project includes an automated visual documentation system to ensure UI consistency across themes and sensors.

### 📸 Sensor Gallery (TOC)
A cross-product of all sensors, widgets, and themes is generated automatically:
- [**View Documentation Gallery**](lib/Sensors/examples/README.md) - Automated visual documentation for all sensors (Real and Virtual).

### ⚙️ How it works
The UI is built using **LVGL**. To verify visual changes without physical hardware, we use a **Simulator Framework**:
1. **Snapshots**: Running `pio test -e simulator` executes a specialized test suite (`test_main.cpp`) that uses the LVGL Snapshot extension.
2. **Matrix Generation**: The test iterates through every sensor, applies every theme, and renders them in every widget type.
3. **Markdown Sync**: The framework automatically updates the individual README files and BMP images in `lib/Sensors/examples/`, ensuring documentation stays in sync with code.

---

## 🧪 Developer Guide

### 1. Adding a New Feature (TDD)
We follow a **Red-Green-Refactor** workflow using the `native` test suite.
1. Add a test in `test/`.
2. Run `pio test -e native` (it should fail).
3. Implement the logic in `lib/`.
4. Run tests again (it should pass).

### 2. Implementing Logic
To add new cross-component behavior:
1. Define the logic in a new `Logic` class in `lib/Logic`.
2. In `main.cpp`, instantiate the logic and pass the required providers from the `MachineFactory`.
3. Dispatch commands inside the `Logic::update()` method.

### 3. Handling Noisy Hardware
- **`ADCRawSource`**: Averages 64 samples at the hardware level.
- **`FilteredSensor`**: A base class providing **EMA Smoothing** and **Display Hysteresis** to stop value "flicker".

### 4. Warmup Detection (Dimensional History)
The `WarmingUpBlocker` uses a robust **Dimensional History** approach:
- **Moves**: Tracks monotonic pressure moves (Cooling/Heating) as "dimensions" in a 2D vector history.
- **Cycles**: Completion is derived from move transitions: `((moves.size() - 1) / 2) >= 3`.
- **UI Feedback**: High-granularity progress (14% steps) reflects every directional flip.
- **Bypass**: Boilers already pressurized (> 0.3 Bar) bypass the warmup phase entirely.

---

---

## 📖 Documentation & Thinking of Others

For more in-depth information and the rules of the codebase, please refer to the following:

- **[📜 Developer Standards & Principles](docs/README.md)**: **Mandatory reading** for all contributors. Covers our TDD charter, Coordinator-Logic patterns, and Agentic rules.
- **[Internal Library Map](lib/README.md)**: A detailed guide to the project's file structure.
- **[GitHub Wiki](https://github.com/richardarpino/Fracino_Cherub_ShotTimer/wiki)**: Technical deep-dives, hardware schematics, and setup guides.
- **[🐞 Issue Tracker](https://github.com/richardarpino/Fracino_Cherub_ShotTimer/issues)**: Found a bug or have a feature request? Please open an issue here!

---

## 🔌 Hardware Reference

Example board used for development: [TENSTAR T-Display ESP32 Development Board with 1.14 Inch LCD](https://www.aliexpress.com/item/1005062610762446.html). PlatformIO environment: `lilygo-t-display` is compatible.

| Pin Mapping | Function | Notes |
| :--- | :--- | :--- |
| **GPIO 32** | Pressure Sensor | Analog 0-3.3V (use voltage divider) |
| **GPIO 25** | Pump Signal | AC Optocoupler (Active LOW/HIGH configurable) |
| **GPIO 35** | Button | Right button on T-Display |
| **GPIO 4** | Backlight | PWM controllable |

---

*Maintained by the [Antigravity](https://deepmind.google/) Agentic AI.*
