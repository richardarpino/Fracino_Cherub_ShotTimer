---
description: The mandatory verification sequence for every implementation phase.
---
# The "Bone Basic" Verification Protocol

Every task or phase must conclude with this sequence. No phase is considered "GREEN" until these steps are executed and their output is verified as successful.

1. **Phase Identification**: Ensure the current code matches the Goal.
2. **RED State (Optional)**: If starting a new feature, run tests to see them fail.
3. **GREEN State (Implementation)**: Apply implementation.
4. **Bone Basic Verification**:
    // turbo
    - Run `pio run -e lilygo-t-display` to verify hardware compilation.
    - Run `pio test -e native` to verify logic regression.
    - Run `pio test -e simulator` to verify UI rendering (if applicable).
5. **Report**: Link the output log in the Walkthrough.

UNACCEPTABLE ACTIONS:
- Claiming success based on "code inspection" alone.
- Skipping the hardware build because "native tests passed".
- Ignoring build warnings.
