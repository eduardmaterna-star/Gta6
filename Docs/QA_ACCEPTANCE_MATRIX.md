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

### Save/load (source slice implemented; engine/runtime acceptance pending)
- Schema version, map identifier, player transform, selected-character ID and UTC timestamp are serialized.
- Unsupported schema, map mismatch, missing required fields and non-finite/zero-scale transforms are rejected before applying player state.
- A valid existing primary slot is copied to the backup slot before primary replacement; a failed backup write aborts replacement.
- Load validates the primary slot and falls back to a validated backup before changing the player pawn.
- Packaged Development tests must prove restart persistence, corrupt/truncated slot recovery, backup fallback and unchanged world state after failed load.
- Unreal Automation tests and future schema migration tests remain required before issue #7 can close.

### Vehicle (source prototype implemented; engine/runtime acceptance pending)
- Vehicle pawn has a physics-simulated chassis, bounded throttle/steering input, braking and a follow camera.
- Entry stores the previous pawn and hands possession to the vehicle; exit checks for a safe teleport spot before restoring possession.
- Reset clears linear and angular velocity only when a valid physics body/world exists and the reset location is not blocked.
- UE 5.8 build and PIE tests must cover enter/exit/re-enter, blocked exit, collision, rollover/reset and level unload.
- Physics stability and performance must be measured at controlled speeds and frame rates. The current prototype is arcade rigid-body handling, not a wheel/suspension vehicle.

### Traffic, police, gangs and missions (not implemented)
- Each system requires explicit state transitions, invalid-input handling and cancellation/cleanup behavior.
- Tests must cover normal, boundary, failure and repeated-trigger scenarios.
- Cross-system events cannot create duplicate pursuits, missions or faction changes.
- No feature is marked complete based only on an empty class, placeholder mesh or documentation.

## Current verification status (2026-11-12)

- Repository static validation: current commit result must be checked in GitHub Actions; checks are source-level only.
- Installer source validation: source configuration checks only; no real game executable is packaged or launched.
- Unreal C++ compilation (including the new persistence subsystem): not run in this environment.
- Unreal Editor/runtime tests: not run.
- Packaged Windows game and offline installation: not verified.
- Performance measurements: not available.

Never replace these statuses with a green badge unless the corresponding test actually ran on the current commit.
