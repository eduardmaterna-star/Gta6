# Milestone 001 — C++ foundation

## Implemented in source

- Third-person player character.
- Camera and movement.
- Legacy input mappings.
- Interaction interface for C++ and Blueprint actors.
- Basic interactable actor.
- GameMode with the default player character.

## Local UE5 acceptance tests

1. Project opens and generates project files without errors.
2. C++ module compiles in Editor Development configuration.
3. Player moves with WASD.
4. Mouse controls the camera.
5. Jump works.
6. E calls the interface on a targeted interactable actor.
7. Non-interactable actors are ignored.
8. Game starts and exits cleanly.

## Not verified

- Unreal Header Tool compilation.
- MSVC linking.
- Unreal Editor runtime.
- FPS, RAM, and VRAM.
- Compatibility with a specific UE5 minor version.