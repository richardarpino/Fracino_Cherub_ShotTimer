# Development Workflow

This project adheres to a strict, data-first development workflow. Following these steps ensures high reliability in a project where hardware debugging is expensive.

## 1. The TDD Charter (Mandatory)
Any logic change, bug fix, or new feature **must** start with a test.

1.  **Red**: Create a test case in `test/` that reproduces the bug or defines the new feature. Run `pio test -e native` to confirm it fails.
2.  **Green**: Implement the minimal code in `lib/` to make the test pass.
3.  **Refactor**: Clean up the code, maintain abstractions, and ensure no regressions.

## 2. Environment Roles
We use three distinct PlatformIO environments. Use the right tool for the job:

### 🚀 `native` (The Workhorse)
*   **Usage**: 95% of your time.
*   **Purpose**: Running unit tests and logic verification.
*   **Note**: It runs on your host machine. It is instant. If it works here, the logic is sound.

### 🖼️ `simulator` (The Studio)
*   **Usage**: UI design and regression.
*   **Purpose**: Generating the Documentation Gallery and verifying UI layouts.
*   **Anti-Pattern**: Do NOT use this to debug machine logic. It’s too slow for logic-loop iteration.

### 🔌 `hardware` (`lilygo-t-display`)
*   **Usage**: Final verification.
*   **Purpose**: Checking real-world sensor noise, SPI timing, and memory usage.
*   **Warning**: If you are debugging logic on hardware, you have skipped the `native` step.

## 3. Sensor Management
*   **Filtering**: New sensors should inherit from `FilteredSensor` to get EMA smoothing and hysteresis for free.
*   **Safety**: Always test edge cases (0.0 values, timeouts, disconnected sensors) in your `native` suite.
