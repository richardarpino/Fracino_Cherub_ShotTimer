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
*   **Safety**: This environment **ignores** UI/LVGL and Themes. It is designed for near-instant feedback on "machine brains" (Logic modules, Sensor math, Dispatcher state).
*   **Validation**: Always run this first. If logic fails here, don't bother with other environments.

### 🖼️ `simulator` (The Studio / Integration Guard)
*   **Usage**: UI design, interface refactors, and regression.
*   **Purpose**: Generating the Documentation Gallery and verifying UI layouts.
*   **Safety**: This environment **includes** the full LVGL stack. It is your **Integration Guard**.
*   **Why run logic tests here?**: While logic is tested in `native`, interface changes (like modifying `IWidget`) can only be caught by the `simulator` build. Running logic tests in this environment ensures that the UI layer hasn't broken the overall system compilation.
*   **Anti-Pattern**: Do NOT use this for rapid logic iteration. The LVGL compilation overhead makes it too slow.

### 🔌 `hardware` (`lilygo-t-display`)
*   **Usage**: Final verification.
*   **Purpose**: Checking real-world sensor noise, SPI timing, and memory usage.
*   **Warning**: If you are debugging logic on hardware, you have skipped the `native` step.

## 3. Verification Strategy

When making changes, follow this tiered verification strategy:

1.  **Logic Changes**: Run `pio test -e native`.
2.  **UI/Interface Changes**: Run `pio test -e simulator` (Essential to catch compilation errors in widgets).
3.  **Documentation Sync**: Verify the gallery in `lib/Sensors/examples/` after a `simulator` run.
4.  **Hardware Release**: Final flash to the machine.
## 3. Sensor Management
*   **Filtering**: New sensors should inherit from `FilteredSensor` to get EMA smoothing and hysteresis for free.
*   **Safety**: Always test edge cases (0.0 values, timeouts, disconnected sensors) in your `native` suite.
