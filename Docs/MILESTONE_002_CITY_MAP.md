# Milestone 002 — first playable city blockout

## Included

- Runtime-generated district approximately 1.1 km across.
- A regular street grid with road surfaces and sidewalks.
- Deterministic building placement and varied building heights.
- A central public park with simple blockout trees.
- Repeated street furniture for scale and navigation cues.
- Automatic player start and third-person pawn from the existing game mode.

## Current limitations

This is a greybox map made from Unreal Engine's built-in cube mesh. It is not a finished or geographically accurate Prague map. Buildings have no interiors, doors, windows, traffic, pedestrians, missions, materials, or detailed props yet. The map is generated at runtime and is intended to validate scale, navigation, and the first playable loop before art production.

## Controls

- W A S D — movement
- Mouse — camera
- Space — jump
- E — interact (only actors implementing UOWInteractable)

## Verification status

Source was reviewed for consistency with the existing module and legacy input setup. Unreal Engine and the matching C++ toolchain are not available in this execution environment, so compilation, editor launch, collision testing, and frame-rate testing remain **not verified**. First validation should be performed in the target UE5 version on Windows.

## Next work

1. Add a real landscape/terrain and material palette.
2. Replace primitive buildings with modular building kits.
3. Add district boundaries, landmarks, intersections, and navigation signage.
4. Add a map overview and player waypoint marker.
5. Profile the generated district on the target PC before increasing density.
