# ADR 0002: Native-First Verification Strategy

*   **Status**: Accepted
*   **Date**: 2026-01-22 (Initial Setup) / 2026-02-25 (Re-assertion)

## Context
From the inception of the project, we established a `native` environment to allow for high-speed logic verification without hardware. However, as the project evolved to include more UI-heavy features:
1.  **Complexity Blur**: A `simulator` environment was introduced (Feb 22, 2026) to generate the visual documentation gallery.
2.  **Mistaken Dependency**: Some logic tests were mistakenly added to the `simulator` suite, leading to slower iteration cycles.
3.  **Ambiguity**: It became unclear which environment was the "Source of Truth" for new developers.

## Decision
We re-asserted the **Native-First Verification Strategy**:
1.  **Logic Isolation**: All domain logic and services (WiFi, OTA, Warmup) must be verified in the `native` environment.
2.  **Simulator Limits**: The `simulator` environment is strictly reserved for **Visual Regression** and **Automated Gallery Generation**. It must not be used to verify machine logic.
3.  **Environmental Discipline**: If a test can run without a display, it belongs in `native`.

## Consequences
*   **Positive**: Maintains extremely fast iteration (0.7s vs 10s simulator).
*   **Positive**: Prevents UI-related bugs from masking underlying logic failures.
*   **Negative**: Developers must maintain high-quality stubs to simulate hardware feedback on the host machine.
