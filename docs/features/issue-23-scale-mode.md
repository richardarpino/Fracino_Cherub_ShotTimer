# Feature: Scale Mode (Issue #23)
Status: In-Progress

## Goal
Provide a dedicated, manual weighing mode ("Scale Mode") for general coffee preparation tasks (e.g., weighing beans, dosing hot water for Americanos, or diagnostic troubleshooting) that is integrated seamlessly with the automated Shot Timer.

## Requirements
- **Manual Activation**: Triggered by a latched toggle on the **Left Onboard Button**. 
- **Immediate Tare**: Upon entering Scale Mode, the weight is automatically tared to `0.0g`.
- **Isolated Display**: Shows the tared weight in a clear, focused screen.
- **Shot Pre-emption**: If the pump is activated while in Scale Mode, the **Shot Timer** workflow takes priority (Precedence 100 vs Scale 50).
- **Dual-Tare Logic (Americano-Safe)**: 
    - The system maintains **two separate tare offsets** simultaneously.
    - **Scale Mode** uses its own offset captured at manual activation.
    - **Shot Timer** uses its own offset captured at pump start.
    - Result: The Shot Timer shows *just the espresso weight* (e.g. 28g), while the Scale Mode shows the *cumulative total* (e.g. 128g) when you fall back to it.
- **Fallback Persistence**: When the pump stops (and the 10s summary delay ends), the engine returns to the Scale Mode screen if it was not manually toggled off.

## Example: The Americano Workflow
1. **Cup on Scale**: Under the hot water tap.
2. **Enter Scale Mode**: Press Left Button. Screen shows **0.0g**.
3. **Add Hot Water**: Tap adds 100g. Screen shows **100.0g**.
4. **Prepare Shot**: Move mug under grouphead.
5. **Start Shot**: Pump starts. **Shot Timer** appears, starting at **0.0g** (fresh tare).
6. **Stop Shot**: Target reached (e.g. **28.9g**). Summary persists for 10s.
7. **Return to Scale**: Summary fades. **Scale Mode** returns, showing **128.9g** (Cumulative total).

## Implementation Details

### Logic Components
1. **ToggleTrigger**: A latched trigger that toggles its state on each button press. It publishes `ManualModeReading (bool)` to the registry.
2. **TaredWeightProcessor**: 
    - Monitors `ManualModeReading`.
    - If `ManualMode` is TRUE, it skips the `tare()` call on `pump.justStarted()`.
    - Always performs `tare()` when `ManualMode` toggles from FALSE to TRUE.

### Workflow Tree
- **Root**
  - **Dashboard (1)**
    - **Scale Mode (50)** - Triggered by ToggleTrigger.
    - **Shot Timer (100)** - Triggered by Pump (Delayed).

## Verification Strategy
- **Unit Tests**:
    - `test_toggle_trigger`: Latched behavior.
    - `test_cumulative_weight`: Suppression of auto-tare during manual mode.
- **Manual Check**:
    - Enter Scale Mode (0.0g) -> Add 100g water -> Start Pump -> Verify Shot Timer starts at 100g (not 0g).
