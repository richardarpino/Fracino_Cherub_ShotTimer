---
description: The mandatory pre-flight check before starting any new piece of work.
---
# The "Safe Start" Protocol

Every new task or session MUST begin with this sequence to ensure the environment hasn't regressed and no "leftovers" from previous work are haunting the project.

1. **Verify Baseline**: 
    - Run `/verify-all` to ensure the current `HEAD` is GREEN.
    - If any phase fails (Hardware, Native, or Simulator), STOP and report the breakage to the user immediately.
2. **Check for "Dirty" State**: 
    - Run `git status` to identify any uncommitted changes or untracked files.
3. **Review Recent Environmental Changes**:
    - Run `git log -n 3 platformio.ini` to see if the build environment was recently modified.
4. **Report to User**:
    - Confirm: "Baseline is GREEN and Working Tree is CLEAN. Ready to proceed." 
    - OR: "I found a regression in [X]. We should address this before starting new work."

UNACCEPTABLE ACTIONS:
- Starting work on a "Red" baseline.
- Ignoring local Git modifications.
- Assuming the environment is correct without verification.
