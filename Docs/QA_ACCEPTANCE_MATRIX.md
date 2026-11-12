# QA acceptance matrix

This matrix is the minimum evidence required before marking a feature complete. Static text checks are useful guardrails, but they do not prove engine compilation or runtime behavior.

## Test layers

| Layer | What it proves | Required evidence |
|---|---|---|
| Static repository checks | Required files/configuration and simple invariants exist | CI output and reproducible script |
| C++ build | UnrealHeaderTool and compiler accept the project | UE 5.8 Development Editor build log |
| Unit/automation tests | Deterministic logic and state transitions behave correctly | Unreal Automation test results |
| Editor functional tests | Actors, navigation, maps and interactions work in a real world | Test map and recorded pass/fail results |
| Packaged smoke test | Packaged Windows build starts and basic play works | Build artifact, launch log and test checklist |
| Performance test | Target hardware meets agreed budgets | Resolution, settings, driver, FPS/frame-time, RAM and VRAM report |
| Installer test | Offline install, launch, upgrade/uninstall work | Clean Windows VM results and SHA-256 |

## Feature acceptance

### Player movement and interaction
- Movement is camera-relative and remains stable at different frame rates.
- Jump starts and stops correctly; movement is blocked by world collision.
- Interaction only triggers a valid interactable actor in range and along the camera trace.
- Invalid/destroyed hit actors are ignored safely.

### Greybox world
- Same generation seed and settings produce the same layout.
- Different seeds produce a valid alternate layout.
- All configured dimensions stay within runtime limits.
- Player can traverse roads, sidewalks and park without collision traps.
- Geometry generation does not silently run repeatedly and duplicate instances.

### Civilian navigation
- Test level contains a built NavMesh and a documented spawn setup.
- Civilian obtains an AI controller or reports a bounded recoverable failure.
- It selects reachable points, moves, stops, and retries when navigation is unavailable.
- Timers are cleared and movement stops when the actor is destroyed or the level unloads.
- A population test verifies the configured cap and records CPU/memory/path request rates.
- The spawner uses bounded location attempts, validates a path from its origin, staggers spawns, and cleans up all owned civilians on EndPlay.

### Save/load (when implemented)
- Save survives application restart.
- Invalid/corrupt/unsupported save data is rejected without destroying the last valid save.
- Player transform and completed mission state are restored consistently.
- Save schema migration is covered by tests.

### Vehicles, police, gangs and missions (when implemented)
- Each system has explicit state transitions, invalid-input handling and cancellation/cleanup behavior.
- Tests cover normal, boundary, failure and repeated-trigger scenarios.
- Cross-system events cannot create duplicate pursuits, missions or faction changes.
- No feature is marked complete based only on an empty class, placeholder mesh or documentation.

## Current verification status (2026-11-12)

- Repository static validation: pending for the current civilian population implementation commit.
- Installer source validation: previously passed on an earlier commit; it only checks source configuration.
- Unreal C++ compilation: not run in this environment.
- Unreal Editor/runtime tests: not run.
- Packaged Windows game and offline installation: not verified.
- Performance measurements: not available.

Never replace these statuses with a green badge unless the corresponding test actually ran on the current commit.
