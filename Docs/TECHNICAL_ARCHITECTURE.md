# OPEN WORLD – Next Gen: technical architecture

## Purpose
This document defines the boundaries between gameplay systems so that the project can grow without turning the GameMode or a single actor into a monolith. It is an implementation plan, not a claim that the listed systems already exist.

## Runtime module boundaries

| Area | Responsibility | Must not own |
|---|---|---|
| Game bootstrap | Select game mode, initialize session and world-level services | Mission rules, police logic, vehicle physics |
| Player | Input, camera, interaction requests, character state | Directly changing mission/gang/police state |
| World | District data, roads, traversal zones, streaming integration | NPC decision logic |
| AI | Civilian perception, movement, schedules and behavior policies | Persistent save-file format |
| Vehicles | Vehicle movement/physics, seats, damage and traffic agents | Police escalation rules |
| Factions | Gang identity, hostility, territory and reputation | UI widgets and save-file I/O |
| Police | Crime reports, response stages, dispatch and pursuit state | Direct low-level player input |
| Missions | Objective graph, prerequisites, outcomes and branching | Direct actor spawning without a world service |
| Persistence | Versioned save data and serialization boundaries | Gameplay decisions |

Keep public headers small. Prefer components and subsystems for reusable services; actors should represent world entities. Avoid circular module dependencies and avoid using global mutable state for gameplay.

## Data flow rules

1. Input and interaction produce requests; the owning gameplay system validates and applies them.
2. AI decisions are separated from movement execution. Navigation failures are recoverable states, not fatal errors.
3. World actors are referenced through stable IDs or validated soft references when persistence is required. Do not serialize raw transient pointers.
4. Save data uses an explicit schema version. Loading an unknown future version must fail safely without overwriting the existing save.
5. Gameplay state changes should be event-driven where practical. Do not add per-frame Tick work for logic that can use timers, delegates or scheduled updates.
6. Expensive work (large population updates, path requests, save preparation) must be bounded and profiled.

## Persistence contract

The first save milestone should persist only stable, player-visible state:
- schema version and save timestamp;
- map identifier and player transform;
- selected character;
- mission progress identifiers and objective states;
- inventory/currency after those systems exist;
- faction/reputation state after the faction model exists.

Use a dedicated save-game data type and a persistence service. Validate map identifiers, finite transform values, array sizes and schema versions before applying loaded data. A failed load must leave the current world state untouched. Keep at least one backup slot or use a write-then-rotate strategy before replacing the last known good save.

## AI and population contract

The current civilian class is a navigation prototype only. Before claiming a populated city:
- add a configured NavMesh or an explicitly validated navigation alternative;
- add a population/spawn owner with a hard maximum and per-district budget;
- spawn only at validated navigation locations;
- handle missing navigation data and failed spawn/possess operations;
- provide deterministic test seeds;
- provide cleanup on district unload and world shutdown;
- measure path-request rate, active AI count, CPU time and memory.

Do not spawn an unbounded number of civilians. Crowd simulation and traffic simulation are separate systems and must have independent budgets.

## Vehicle, police, faction and mission sequencing

Implement in vertical slices, not as empty class scaffolds:
1. One drivable test vehicle with input, camera handoff, collision and exit/enter validation.
2. A small road graph and a limited traffic-agent test with stuck detection and safe despawn.
3. A crime-event interface and police response state machine with explicit transitions and cooldowns.
4. Faction definitions and a relationship matrix before implementing gang encounters.
5. Mission objectives as data-driven state machines with explicit success/failure/cancel states.
6. Connect systems through events and validated interfaces, not hard-coded cross-calls.

## Performance rules

- Establish a baseline before adding large populations or streaming complexity.
- Record average, 1% low and frame-time percentiles, plus RAM/VRAM and active actor counts.
- Profile Development builds first; do not infer packaged performance from editor-only runs.
- Treat 1080p/120 FPS as a measured target, not a guarantee.
- Use instancing for repeated static props, but profile collision and draw-call costs separately.

## Current implementation boundary (2026-11-12)

Implemented in source:
- third-person movement and camera;
- line-trace interaction interface dispatch;
- runtime-generated greybox district using instanced cube meshes;
- timer-driven civilian wandering prototype using AIController and NavMesh queries;
- static repository validation and installer-source validation.

Not implemented or not verified:
- real editor-authored World Partition map;
- configured NavMesh and automatic civilian population;
- save/load;
- vehicles and traffic;
- police and gangs;
- mission framework and branching story;
- three playable characters;
- UnrealHeaderTool/MSVC build, editor launch, packaged game and runtime performance tests.

This list is intentionally conservative. A source class or placeholder asset is not counted as a completed gameplay feature.
