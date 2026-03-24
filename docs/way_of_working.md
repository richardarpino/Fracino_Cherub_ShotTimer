# Way of Working: Verification-First Development

## 1. Scope & Strategy
- **One Feature per Branch**: Each feature (Issue) is developed in its own branch.
- **Problem & Goal Defined**: Features are described as a Problem and a Goal, allowing for exploration before implementation.
- **Granular Iterations**: Large features are broken down into discrete iterations (changesets).
- **No Scope Creep**: Additional features or "nice-to-haves" must be moved to new Issues.

## 2. The Development Cycle (RED-GREEN-BLUE)
1. **Research & Discussion (Feature Planning)**: 
   - Analyze the Problem/Goal.
   - Weigh multiple architectural approaches in a `Feature_Planning_Doc.md`.
   - **Approval Required**: Do not proceed to planning without consensus on the approach.
2. **Implementation Planning**:
   - Create `implementation_plan.md` for the *current iteration only*.
   - Identify any existing tests that require amendment.
   - **Approval Required**: No code is written before plan approval.
3. **RED Phase**:
   - Write a failing test (Native for logic, Simulator for UI).
4. **GREEN Phase**:
   - Implement the minimum code to pass the test.
   - **Halt on Drift**: If passing the test requires changing the plan or existing tests (including transitive regressions), **STOP** and seek approval.
5. **BLUE Phase (Refactoring)**:
   - Clean up code and ensure adherence to project patterns.
   - Led by the human or guided by the AI with strict oversight.

## 3. Verification & Commitment
Each iteration must complete a "verify-all" pass before commitment:
1. `pio test -e native` (Logic)
2. `pio run -e lilygo-t-display` (Build/Syntax)
3. `pio test -e simulator` (UI structure)
4. **Hardware Soak Test**: Mandatory verification on the production board (e.g., "Cup On/Off" test).

### Commitment Rules:
- **Checkpoint Commit**: Local commit after every GREEN phase.
- **The Commit Gate**: AI must explicitly present the staging area (`git status`) and request a "Proceed to Commit" from the Human. **Zero autonomy on commits.**
- **Iteration Commit**: Final commit only after successful verify-all/soak test.
- **Ghost Code Prevention**: Run `pio run -e native` immediately after any `git` operation.

## 4. Problem Resolution
- **Test-Driven Debugging**: Every bug must be exposed by a failing test.
- **Legacy Failure Protocol**: If a non-related test fails, do not ignore or delete it. Perform a "Divergence Report" to explain why it's failing and propose a fix/relocation before proceeding.
- **Strict Test Invariance**: Behavioral tests are contracts. If a code change breaks a test, do **NOT** update the expectation to match the NEW code. You must first provide a "Contract Breach Report" justifying why the previous behavior is obsolete. If both behaviors remain valid, the logic must be abstracted and a new test suite added.
- **3-Attempt Limit**: If a problem cannot be derived or fixed in 3 attempts, halt and discuss.
- **No Guessing**: Changes must be justified by evidence (compiler errors or failing tests).

## 5. Local Tooling & Environment
- **PlatformIO Shim**: Always use the `./pio` script in the root to ensure environment parity.
