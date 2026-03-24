# Feature Planning: Scale Integration (Issue #22)

## 1. Problem Statement
Measuring espresso by weight (yield) is the gold standard for consistency. External scales are logically decoupled from the machine's state, take up valuable vertical space, and are often messy in a drip-tray environment. Integrating load cells directly into the machine's frame/drip-tray assembly provides a seamless, automated way to track shot yield.

## 2. Goal
The primary objective of this phase is to integrate HX711-based load cells into the `SensorRegistry`. In line with the **Pragmatic Evolution** principle (Rule Zero), we will focus on **Iterative Discovery**:
- **Phase 1 (Current)**: Solid Raw Readings.
- **Phase 2 (Workflows)**: Taring, Calibration, and Workflow integration.

---

## 3. Discussion Points & Technical Context

### A. Hardware Pin Identification
Based on the TENSTAR T-Display pinout, we will use the following **GPIO numbers** (which correspond to the labels in the red/purple bubbles in the hardware diagram):

- **Proposal**: **GPIO 39** (Hole labeled `39`) for DOUT and **GPIO 33** (Hole labeled `33`) for SCK.
- **Rationale**: 
    - **Proximity**: These pins are physically located in the same "sensor block" on the right-hand header, adjacent to the existing Pressure (GPIO 32) and Pump (GPIO 25) connections.
        - Physical Pin 6: GPIO 39 (DOUT)
        - Physical Pin 7: GPIO 32 (Pressure)
        - Physical Pin 8: GPIO 33 (SCK)
        - Physical Pin 9: GPIO 25 (Pump)
    - **Capability**: GPIO 39 is input-only (perfect for DOUT). GPIO 33 is a general IO (suitable for SCK).
    - **Isolation**: These pins avoid the internal conflicts (Display/Buttons) that typically affect GPIO 0, 4, 16, 17, etc., on this board.

#### Backup Pin Options
In case the primary pins are physically damaged or occupied, here are recommended backup sets:

| Set | DOUT (Input) | SCK (Output) | Rationale |
| :--- | :--- | :--- | :--- |
| **A (Front Side)** | **GPIO 36** | **GPIO 26** | Also on the right header; uses clean digital IOs. |
| **B (Back Side)** | **GPIO 13** | **GPIO 17** | Uses the left-hand header; avoids all strapping pin issues at boot. |
| **C (I2C Area)** | **GPIO 21** | **GPIO 22** | Safest digital IOs if you aren't planning to add I2C sensors later. |

**Note**: In all code and configuration, we refer to the **GPIO number**, not the physical pin count from the top of the header.

### B. Non-Blocking Acquisition ("Machine Feel")
The HX711 has a slow conversion rate (~10Hz or 80Hz). A blocking `read()` would stall the main loop for up to 100ms, which would break the "feel" of the UI and potentially delay safety-critical pressure monitoring.
- **Strategy**: The `HX711Source` will poll `digitalRead(DOUT)`. If the pin is HIGH (not ready), the system will move on immediately and continue the loop. This ensures the machine remains responsive at 60Hz+.

### C. Custom Implementation vs. Library
While several libraries (e.g., `HX711-arduino`) exist, we are choosing a custom `HX711Source` implementation for the following reasons:
1.  **Guaranteed Non-Blocking**: Many libraries default to blocking `read()` calls or `while(!is_ready())` loops. A custom polling state machine ensures absolute compatibility with the project's 60Hz UI and safety-critical pressure monitoring.
2.  **Architectural Fit**: It allows direct mapping to the `IRawSource` interface, avoiding the overhead of "wrapping a wrapper."
3.  **Testability**: We can trivially mock the HX711 bit-stream in `native` tests to verify count reconstruction without external dependencies.
4.  **Minimalism**: The protocol is a simple 24-bit shift register. Implementing it directly is cleaner than managing a library dependency for such a primitive operation.

### D. Calibration & Persistence
The user raised the point: "where should calibration values live given it will be different for different load cells?"
- **Current Pattern**: Other sensors (e.g. Pressure) use hardcoded scalars in `pins.h`.
- **Discussion**: For **Iteration 1 (Raw Focus)**, we could follow the `pressureScalar` pattern to get data flowing quickly. However, since load cells vary significantly and might require field-recalibration, we should consider a move towards **NVS (Non-Volatile Storage)**. 
- **Question**: Should we introduce NVS now, or "put it in the wrong place" (hardcoded) first to prove the raw data pipeline?

---

## 4. Refined Iteration Strategy

### Iteration 1: Single Cell End-to-End
- **Scope**: One load cell, one HX711, and the primary pins (39/33).
- **Goal**: Get a fluctuating raw value in the `SensorRegistry` that responds to physical pressure.
- **Verification**: `native` tests for the `HX711Source` state machine and a `hardware` test to see counts on the Serial monitor.

### Iteration 2: Dual Cell Integration & Summing
- **Scope**: Parallel wiring of the second load cell.
- **Goal**: Handle the physical/software transition to a dual-cell setup and implement initial calibration.
- **Verification**: Stable readings with weight placed anywhere on the tray (verifying the parallel bridge summing).

---

## 5. Technical Critique of Dual Cell Approach

### A. Wiring Method (Parallel Bridge vs. Dual Module)
- **Assumption**: "In parallel" usually means wiring the Wheatstone bridge outputs of both 5kg/1kg cells into a single HX711 input (Red-to-Red, White-to-White, Green-to-Green, Black-to-Black).
- **Advantage**: Zero software changes required to "see" the second cell. The HX711 simply integrates the combined voltage.
- **Risk**: If the load cells are significantly mismatched in resistance, one may dominate the signal or cause drift. Most coffee scale cells are designed for parallel pairing.
- **Alternative**: Using a second HX711 module. 
    - *Constraint*: We would need a second DOUT pin (e.g. GPIO 36). They can share the SCK pin (GPIO 33). This is more complex but allows software to detect "off-center" loading.

### B. Software Calibration Logic
- **Constraint**: Once the second cell is added, the **Calibration Scalar** will change.
- **Approach**: Keep the software "dumb" in Iteration 1 (just raw counts). In Iteration 2, we introduce the `LinearProcessor` that applies: `(Raw - Zero) * Scalar`.

---

## 6. Verification Plan (Refined)

### Iteration 1 (Raw)
- **Primary**: `SensorRegistry` update confirms `RawWeightTag` is receiving data.
- **Secondary**: Serial output confirms the 100ms non-blocking poll is working (the machine loop frequency remains high).

### Iteration 2 (Calibrated)
- **Primary**: Apply a 100g weight. Verify the registry shows ~100.0 (even if unscaled).
- **Secondary**: Verify that placing the weight on the left vs. right side of the tray yields the same total sum (checks parallel bridge parity).

---

## 7. Calibration Process (Production)

To maintain accuracy as the physical tray mount evolves, a permanent diagnostic path is provided via the `CALIBRATE_SCALE` build flag.

### A. Enabling Calibration Mode
In `platformio.ini`, uncomment the following line to enable raw count logging:
```ini
build_flags = 
    ...
    -D CALIBRATE_SCALE=1
```

### B. Calibration Procedure
1.  **Zero Tare**: Record the `[SCALE] Raw Counts` value from the Serial Monitor with an empty tray. This is your `weightZeroOffset`.
2.  **Known Weight**: Place a calibrated weight (e.g., 500g) on the tray and record the new raw value.
3.  **Calculate Scalar**:
    - `delta = RawValueWithWeight - weightZeroOffset`
    - `weightScale = KnownWeight / delta`
4.  **Update Config**: Apply these values to `include/pins.h`.

### C. Implementation Detail
The calibration logger in `MachineFactory.cpp` is throttled to 500ms and guarded by `#ifdef CALIBRATE_SCALE` to ensure it does not interfere with the performance of the production loop or the stability of the OTA service.

---

## 8. UX Refinements: Shot Summary (Iteration 4)

Real-world testing revealed two critical UX "teething" problems during espresso extraction:

### A. The "Disappearing Timer" Problem
- **Observation**: The Shot Timer screen transitions back to the Dashboard immediately (or within ~1.5s) after the pump stops. This is too fast for the user to record the final yield and time.
- **Requirement**: The Shot Timer screen must persist for a configurable duration (e.g., 7-10 seconds) after the pump stops to serve as a **Shot Summary**.

### B. The "Solenoid Purge" Problem
- **Observation**: When a 3-way solenoid machine stops, it vents pressure into the drip tray. Since the scales are in the tray, this "purge water" adds 2g-5g of weight to the final reading, corrupting the yield data.
- **Requirement**: The yield weight must **freeze** at the exact moment the pump signal is lost. Any subsequent weight changes (solenoid purge, cup removal) must be ignored while the Summary is displayed.

### C. Proposed Logic Change
- **TaredWeightProcessor**: Will monitor the `justStopped()` edge of the pump. Upon stopping, it will capture a `finalWeight` and enter a "Frozen" state.
- **WorkflowEngine**: Will be adjusted to allow the Shot Workflow to remain "Active" (based on a timer) even after its primary trigger (Pump) has cleared.
