---
description: Switch to a newly discovered priority without losing current progress.
---
# The `/pivot` Protocol

Use this when an investigation reveals a more urgent issue or a better path forward than the one currently being pursued.

1. **Snapshot Current Progress**:
    - Create a brief "Pivot Note" in the current artifact directory (e.g., `pivot_from_X.md`).
    - Note the current `HEAD`, the files modified, and the reason for the pivot.
2. **Clear the Deck**:
    - Mark the active `task.md` as `[PAUSED]` or `[SUPERSEDED]`.
    - Archive the current `implementation_plan.md` by renaming it to `implementation_plan_PAUSED.md`.
3. **Instantiate New Priority**:
    - Start a new `task.md` for the new investigation/feature.
    - Create a new `implementation_plan.md` once the path is clear.
4. **State the New Goal**:
    - Summarize: "We are pivoting from [Old Goal] to [New Goal] because [Key Evidence/Discovery]."
5. **Proceed**:
    - Wait for user acknowledgment before making code changes.

# Returning from a Pivot

To return to the paused work:
1. Run `/sync-context`.
2. Delete the "Pivot Note".
3. Rename `implementation_plan_PAUSED.md` back to `implementation_plan.md`.
4. Update `task.md` to remove the `[PAUSED]` marker and resume steps.
