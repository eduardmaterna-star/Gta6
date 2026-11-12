# Civilian population test setup

## What is implemented

`AOWCivilianPopulationSpawner` is a bounded, timer-driven actor that owns a small test population. It is deliberately separate from the civilian's own wandering behavior.

- Default cap: 12 active civilians; editable range 0–50.
- Spawning is staggered by a timer rather than performed in one frame.
- Candidate points use a seeded random stream, are projected onto NavMesh, and are checked for a path from the spawner.
- Location attempts are bounded (1–12 per spawn attempt).
- Missing navigation, invalid class, path failure or spawn collision are recoverable and retried after a delay.
- The spawner tracks only actors it created and destroys those actors when it ends play.
- The actor has no per-frame Tick.

## Required Unreal Editor setup

The repository does not contain an editor-authored test map yet. To exercise this actor in UE 5.8:

1. Create or open a test level with walkable floor geometry.
2. Add a `NavMeshBoundsVolume` covering the walkable test area and build navigation.
3. Verify the NavMesh is visible in the editor navigation visualization.
4. Place one `AOWCivilianPopulationSpawner` actor on the test floor.
5. Keep the default cap at 12 for the first smoke test; then test 0, 1, 12 and a higher configured cap.
6. Play in Editor and verify civilians spawn gradually, move, never exceed the configured cap, and are removed when the spawner/level ends.
7. Remove or disable the NavMesh and verify the actor logs one warning and retries at the configured interval without a rapid loop.

## Determinism and limits

The seed makes candidate sample positions and spawn yaw repeatable for a fixed sequence of successful/failed attempts. NavMesh projection and path query results still depend on the exact level geometry, navigation build and engine version; this is not a promise of bit-identical world simulation across machines.

The path check is synchronous and bounded to at most 12 candidates per spawn cycle. Keep the default population modest and profile before raising the cap. This prototype is not a full crowd simulator, schedule system or traffic system.

## Verification boundary

Static source checks can confirm that the implementation has a hard cap, bounded attempts, path validation and timer cleanup. Only a UE 5.8 build and editor runtime test can confirm compilation, possession, collision placement, actual NavMesh behavior and CPU/memory cost.
