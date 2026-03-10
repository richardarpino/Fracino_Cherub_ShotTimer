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

The project follows a **Reactive Registry** pattern to ensure data integrity and host-independent testing.

### 🧠 Logic Layer (`lib/Logic`)
The "Brain" of the machine.
- **Processors**: Reactive components (`ShotMonitor`, `BoilerTemp`) that transform hardware signals into machine-domain data.
- **Sensor Registry**: The single source of truth. Data is **Published** here by hardware or processors, and **Consumed** by the UI.

### 🏭 Provider Layer (`lib/Factories`)
- **MachineFactory**: Wires concrete hardware to logic via Dependency Injection.
- **Deployment**: Logic is tested 100% in the `native` environment before touching hardware.

---

For a deep dive into the code structure and how components are linked, see the **[Internal Library Map](lib/README.md)**.

---

### 📸 UI Snapshot Galleries
The project maintains pixel-perfect visual documentation of all widgets and user journeys. These are captured automatically by the **Simulator Framework** (`test_main.cpp`) to ensure visual fidelity without physical hardware.
- [**Explore All UI Galleries**](docs/README.md)

---

## 📖 Documentation & Thinking of Others

- **[📜 ADR Index (Architecture Decision Records)](docs/adr/README.md)**: The "Why" behind the "How." A chronological log of every high-impact design choice.
- **[🧭 Developer Standards & Principles](docs/README.md)**: Mandatory reading covering TDD, Coordinator-Logic patterns, and Agentic rules.

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
