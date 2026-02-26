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

### 🔌 Layer 1: Hardware & Interfaces (`lib/Interfaces` & `lib/Sensors/Hardware`)
Everything that measures or detects a physical signal.
- **`HardwareSensor`**: Root base class for all physical sensors (e.g., `BoilerPressure`). Enforces pasivity and EMA filtering.
- **`IRawSource`**: Abstract interface for physical ADCs/GPIOs.
- **Switches**: Binary states with edge detection (`HardwareSwitch`, `DebouncedSwitch`).

### 🧠 Layer 2: Logic Modules (`lib/Logic`)
The **"Brain"** of the machine. It processes hardware signals into machine-domain data.
- **Derived Components**: `BoilerTemperature`, `ManualPumpTimer`, `TaredWeight`. These are **NOT** sensors; they are logical producers.
- **Orchestrators**: `ScaleLogic` coords behavior (e.g., "If pump starts, start shot timer and publish results to Registry").
- **`SensorDispatcher`**: The central **Registry** (Single Source of Truth). Logic modules *push* data here; the UI *pulls* it.

### 🏭 Layer 3: Providers & Factories (`lib/Factories`)
Manages lifetime and wiring.
- **`MachineFactory`**: The single point of instantiation. It wires the concrete hardware to the orchestrators.
- **`IMachineProvider`**: Segregated interfaces (`ISensorProvider`, `ISwitchProvider`) used to access the global Registry and hardware switches.

---

## 🎨 UI Gallery & Automated Documentation

The project includes an automated visual documentation system to ensure UI consistency across themes and sensors.

### 📸 Sensor Gallery (TOC)
A cross-product of all sensors, widgets, and themes is generated automatically:
- [**View Documentation Gallery**](lib/Sensors/examples/README.md) - Automated visual documentation driven by **Type-Tag Domain Contracts**.

### ⚙️ How it works
The UI is built using **LVGL**. To verify visual changes without physical hardware, we use a **Simulator Framework**:
1. **Snapshots**: Running `pio test -e simulator` executes a specialized test suite (`test_main.cpp`) that uses the LVGL Snapshot extension.
2. **Matrix Generation**: The test iterates through every sensor, applies every theme, and renders them in every widget type.
3. **Markdown Sync**: The framework automatically updates the individual README files and BMP images in `lib/Sensors/examples/`, ensuring documentation stays in sync with code.

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
