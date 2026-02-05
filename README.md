# Fracino Cherub Shot Timer & Boiler Monitor

This project is a custom ESP32-based mod for the Fracino Cherub espresso machine. It provides a shot timer and a real-time display of the boiler pressure and estimated temperature.

## Features
- **Auto-Shot Timer**: Detects when the pump is active (via GPIO) and automatically starts/stops the timer.
- **Boiler Pressure Monitoring**: Reads a 0.5-4.5V pressure transducer (via voltage divider) to display pressure in Bar.
- **Temperature Estimation**: Calculates the internal boiler temperature from the pressure reading using the **Antoine Equation** (for saturated steam).
- **Split-Screen UI**: 
  - **Left**: Boiler Status (Pressure & Temperature).
  - **Right**: Shot Duration (Seconds).
- **Theme Support**: Multiple visual themes (Default, Candy, Christmas) switchable via button.

## Hardware
- **MCU**: ESP32 (e.g., TTGO T-Display with built-in TFT).
- **Pressure Sensor**: 5V Transducer (0.5V - 4.5V output). 0.5Mpa Range (0-5 Bar). 130 deg C max temp.
- **Connection**:
  - **Pump Signal**: GPIO 21 (Input Pullup).
  - **Button**: GPIO 35 (Theme toggle).
  - **Pressure Sensor**: GPIO 34 (ADC Input). **REQUIRES VOLTAGE DIVIDER** (4.7kΩ + 10kΩ) to step 4.5V down to ~3.0V.

## Code TL;DR
Quick guide to the codebase structure:

| File | Description |
| :--- | :--- |
| **`src/main.cpp`** | **Entry Point**. Sets up hardware, runs the main loop, handles theme switching, and coordinates the Timer/Display/Sensors. |
| **`lib/BoilerPressure/`** | **Sensor Logic**. Handles ADC reading, voltage conversion, smoothing (EMA), and temperature calculation. |
| **`lib/ShotTimer/`** | **Timing Logic**. State machine (READY -> RUNNING -> FINISHED) managing the shot duration logic. |
| **`lib/ShotDisplay/`** | **UI Rendering**. Handles drawing the split-screen layout, fonts, and manual graphics (like the degree symbol). |
| **`lib/Themes/`** | **Styling**. Interface (`ITheme`) and concrete themes defining colors. |

## Quick Start
1.  **Check Wiring**: Ensure the pressure sensor is connected via the voltage divider to **GPIO 34**.
2.  **Build**: Run `pio run` or use the PlatformIO Build button.
3.  **Upload**: Connect ESP32 and upload.
4.  **Verify**: Screen should show ~0.0 Bar / 25°C when cold/empty, and rise to ~1.2 Bar / 123°C when at operating pressure.
