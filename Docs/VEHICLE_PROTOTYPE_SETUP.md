# First drivable vehicle prototype

## Scope

The first vehicle slice adds a physics-simulated pawn with forward/reverse throttle, steering torque, braking and lateral velocity damping; a follow camera; entry through the existing E interaction interface and controller possession handoff; safe exit using a world teleport-spot check; and an explicit ResetVehicle recovery function.

This is an arcade-style rigid-body prototype using a placeholder chassis. It is not a production wheel/suspension vehicle and does not use Chaos Vehicles. No traffic agents or road graph are implemented yet.

## Editor setup

1. Open the project in Unreal Engine 5.8.
2. Build the OpenWorldNextGen module.
3. Place AOWVehiclePawn in a test level on a flat collision-enabled surface.
4. Ensure the player can reach the chassis with the existing E interaction trace.
5. Start PIE and press E while looking at the vehicle to enter.
6. Drive with W/S, steer with A/D, hold Space to brake, and press E to exit.
7. Check that exit is refused when no safe position is available.
8. Call ResetVehicle from an authorized test/debug Blueprint or C++ harness after a controlled stuck/rollover scenario.

## Input mappings

- VehicleThrottle: W = +1, S = -1
- VehicleSteer: D = +1, A = -1
- VehicleBrake: Space

The original character movement mappings remain unchanged. The vehicle reuses the existing Interact action for entry/exit.

## Limitations and acceptance still required

- Tune forces, grip, braking and steering in UE 5.8; source defaults are starting values only.
- Confirm physics stability at multiple frame rates and with collision impacts.
- Verify camera handoff, enter/exit, blocked exit, reset and level unload in PIE and packaged Development builds.
- Add Unreal Automation/functional tests.
- Implement a bounded road graph and traffic agents only after the single-vehicle behavior is stable.
- Do not claim performance or vehicle handling quality until measured.
