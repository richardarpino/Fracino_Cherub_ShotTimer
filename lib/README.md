# 📦 Internal Library Structure

This directory contains the core machine logic, hardware abstractions, and UI components. We follow a **Pure Factory** and **Coordinator-based** architecture.

## 📂 Directory Guide

### 🏭 `Factories/`
The "Assembly Line" of the machine.
- **`MachineFactory`**: The single source of truth for hardware. It handles pin mappings and component lifetimes.
- **Pattern**: Uses **Lazy Instantiation**. Hardware services (WiFi, OTA) only start when a logic module first requests them.

### 🔌 `Interfaces/`
The "Contract Layer". All logic modules depend on these abstractions, not on concrete hardware.
- **`ISensor` / `ISwitch`**: Base protocols for reading states and detecting edges.
- **`IMachineProvider`**: Segregated interfaces (`ISensorProvider`, `ISwitchProvider`) that provide access to hardware.

### 🧠 `Logic/`
The "Brain" of the machine. Contains orchestrators that coordindate between sensors.
- **`StartupLogic`**: Manages the boot sequence and network status.
- **`ScaleLogic`**: Coordinates the pump signal with the shot timer and taring logic.
- **`ThemeManager`**: Handles visual state transitions.

### 🌡️ `Sensors/`
Hardware and software-based measurement components.
- **`Hardware/`**: Physical inputs like `BoilerPressure`, `DigitalRawSource`, and `WiFiSensor`.
- **`Virtual/`**: Logic-based sensors like `DebouncedSwitch` (decorator) or `TaredWeight`.

### 🛰️ `Services/`
Active system-level components.
- **`OTAService`**: Handles wireless updates. Encapsulates its own hardware listeners.

### 🎨 `Themes/`
Visual styling and branding.
- **`Colors.h`**: Hardware-agnostic 16-bit color tokens.
- **`ITheme`**: Interface for visual skins (`CandyTheme`, `ChristmasTheme`).

### 📱 `UI/` & `ShotDisplay/`
The presentation layer built on **LVGL**.
- **`UI/`**: Reusable widgets like `SensorWidget`.
- **`ShotDisplay/`**: The main display coordinator and screen layouts.

---

## 🛠 Developer Workflow

1. **Find an Interface**: Look in `lib/Interfaces` to see if a contract already exists.
2. **Add a Component**: Place concrete hardware in `lib/Sensors/Hardware` and wire it into the `MachineFactory`.
3. **Add Logic**: Create an orchestrator in `lib/Logic` that uses the `IMachineProvider` to get components.
4. **Test**: Always add a native test in `test/` verifying your logic using stubs.
