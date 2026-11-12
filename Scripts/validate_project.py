#!/usr/bin/env python3
"""Fast repository sanity checks; this does not replace an Unreal Engine build."""
from __future__ import annotations

import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    project_path = ROOT / "OPEN_WORLD_Next_Gen.uproject"
    project = json.loads(project_path.read_text(encoding="utf-8"))
    require(project.get("EngineAssociation") == "5.8", "Project must target UE 5.8.")
    modules = project.get("Modules", [])
    require(any(m.get("Name") == "OpenWorldNextGen" and m.get("Type") == "Runtime" for m in modules),
            "Runtime module OpenWorldNextGen is missing.")
    plugins = {p.get("Name"): p.get("Enabled") for p in project.get("Plugins", [])}
    require(plugins.get("PCG") is True, "Built-in PCG plugin must be enabled.")

    build_cs = ROOT / "Source/OpenWorldNextGen/OpenWorldNextGen.Build.cs"
    game_target = ROOT / "Source/OpenWorldNextGen.Target.cs"
    editor_target = ROOT / "Source/OpenWorldNextGenEditor.Target.cs"
    for path in (build_cs, game_target, editor_target):
        require(path.is_file(), f"Missing build file: {path.relative_to(ROOT)}")

    engine_config = (ROOT / "Config/DefaultEngine.ini").read_text(encoding="utf-8")
    require("/Script/OpenWorldNextGen.OWGameMode" in engine_config,
            "Default game mode path is missing from DefaultEngine.ini.")

    required_sources = (
        "Source/OpenWorldNextGen/OpenWorldNextGen.cpp",
        "Source/OpenWorldNextGen/Game/OWGameMode.h",
        "Source/OpenWorldNextGen/Game/OWGameMode.cpp",
        "Source/OpenWorldNextGen/Player/OWPlayerCharacter.cpp",
        "Source/OpenWorldNextGen/World/OWCityMap.cpp",
        "Source/OpenWorldNextGen/AI/OWCivilianCharacter.cpp",
        "Source/OpenWorldNextGen/AI/OWCivilianPopulationSpawner.h",
        "Source/OpenWorldNextGen/AI/OWCivilianPopulationSpawner.cpp",
        "Source/OpenWorldNextGen/Vehicles/OWVehiclePawn.h",
        "Source/OpenWorldNextGen/Vehicles/OWVehiclePawn.cpp",
    )
    for relative in required_sources:
        require((ROOT / relative).is_file(), f"Missing required source: {relative}")

    game_mode_header = (ROOT / "Source/OpenWorldNextGen/Game/OWGameMode.h").read_text(encoding="utf-8")
    game_mode_source = (ROOT / "Source/OpenWorldNextGen/Game/OWGameMode.cpp").read_text(encoding="utf-8")
    city_map_source = (ROOT / "Source/OpenWorldNextGen/World/OWCityMap.cpp").read_text(encoding="utf-8")

    require("virtual void InitGame(" in game_mode_header,
            "GameMode must declare InitGame so the fallback PlayerStart exists before player spawning.")
    init_game_index = game_mode_source.find("void AOWGameMode::InitGame(")
    begin_play_index = game_mode_source.find("void AOWGameMode::BeginPlay(")
    player_start_index = game_mode_source.find("SpawnActor<APlayerStart>")
    require(init_game_index >= 0 and begin_play_index > init_game_index,
            "GameMode InitGame must be implemented before BeginPlay.")
    require(init_game_index < player_start_index < begin_play_index,
            "Fallback PlayerStart must be spawned during InitGame, not after BeginPlay.")
    require("TActorIterator<APlayerStart>" in game_mode_source,
            "GameMode must reuse a level-authored PlayerStart instead of creating a duplicate.")
    require("SpawnActor<APlayerStart>" not in city_map_source,
            "City map must not create PlayerStart during its late BeginPlay.")
    require("TActorIterator<AOWCityMap>" in game_mode_source,
            "GameMode must reuse an editor-authored city map instead of spawning duplicate runtime geometry.")
    require("failed to spawn the fallback city map" in game_mode_source,
            "GameMode must report a failed fallback city-map spawn.")

    input_config = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
    for mapping in ("MoveForward", "MoveRight", "Turn", "LookUp", "Jump", "Interact"):
        require(mapping in input_config, f"Missing default input mapping: {mapping}")

    player_source = (ROOT / "Source/OpenWorldNextGen/Player/OWPlayerCharacter.cpp").read_text(encoding="utf-8")
    interactable_header = ROOT / "Source/OpenWorldNextGen/Player/OWInteractable.h"
    require(interactable_header.is_file(), "Player interaction interface header is missing.")
    require("IOWInteractable::Execute_Interact" in player_source,
            "Player interaction must dispatch through the generated interface wrapper.")

    require("StreetFurniture.Get(), ParkProps.Get()" in city_map_source,
            "Decorative components must be configured separately from collidable world geometry.")
    civilian_header = (ROOT / "Source/OpenWorldNextGen/AI/OWCivilianCharacter.h").read_text(encoding="utf-8")
    civilian_source = (ROOT / "Source/OpenWorldNextGen/AI/OWCivilianCharacter.cpp").read_text(encoding="utf-8")
    require("AOWCivilianCharacter" in civilian_header and "AOWCivilianCharacter::ChooseNextWanderDestination" in civilian_source,
            "Civilian navigation prototype is missing.")
    require("AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned" in civilian_source,
            "Civilian actors must automatically receive an AI controller when placed or spawned.")
    require("GetRandomReachablePointInRadius" in civilian_source and "MoveToLocation" in civilian_source,
            "Civilian wandering must use reachable NavMesh destinations.")
    require("GetMoveStatus() == EPathFollowingStatus::Moving" in civilian_source,
            "Civilian wandering must not replace an active movement request.")
    require("EPathFollowingRequestResult::Failed" in civilian_source,
            "Civilian wandering must retry safely when a movement request fails.")
    require("MinimumWanderDistance" in civilian_header and "DistSquared2D" in civilian_source,
            "Civilian wandering must reject destinations that are too close to create visible movement.")
    require("bUseRVOAvoidance = true" in civilian_source and "AvoidanceConsiderationRadius" in civilian_source,
            "Civilian movement must enable basic local RVO avoidance.")
    require("PrimaryActorTick.bCanEverTick = false" in civilian_source,
            "Civilian prototype should remain timer-driven without an unnecessary per-frame tick.")
    require("ClearTimer(WanderTimerHandle)" in civilian_source,
            "Civilian timer must be cleared on EndPlay.")

    population_header = (ROOT / "Source/OpenWorldNextGen/AI/OWCivilianPopulationSpawner.h").read_text(encoding="utf-8")
    population_source = (ROOT / "Source/OpenWorldNextGen/AI/OWCivilianPopulationSpawner.cpp").read_text(encoding="utf-8")
    require("MaxPopulation = 12" in population_header and 'ClampMax = "50"' in population_header,
            "Civilian population must have a conservative, editor-configurable hard cap.")
    require("FMath::Clamp(MaxPopulation, 0, 50)" in population_source,
            "Civilian population cap must also be enforced at runtime.")
    require("MaxLocationAttemptsPerSpawn" in population_header and "FMath::Clamp(MaxLocationAttemptsPerSpawn, 1, 12)" in population_source,
            "Civilian spawn location queries must have a bounded retry count.")
    require("RandomStream.Initialize(RandomSeed)" in population_source and "RandomStream.FRandRange" in population_source,
            "Civilian spawn candidate sampling must use a configurable deterministic seed.")
    require("ProjectPointToNavigation" in population_source and "GetPathLength" in population_source,
            "Civilian spawn points must be projected onto NavMesh and checked for a connecting path.")
    require("SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding" in population_source,
            "Civilian spawning must not force actors into overlapping collision.")
    require("ScheduleNextSpawn(RetryInterval)" in population_source,
            "Failed civilian spawn attempts must use a delayed retry.")
    require("ClearTimer(SpawnTimerHandle)" in population_source and "Civilian->Destroy()" in population_source,
            "Population spawner must clear its timer and clean up owned civilians on EndPlay.")
    require("PrimaryActorTick.bCanEverTick = false" in population_source,
            "Population spawner must remain timer-driven without an unnecessary per-frame tick.")

    require("\"AIModule\"" in (ROOT / "Source/OpenWorldNextGen/OpenWorldNextGen.Build.cs").read_text(encoding="utf-8"),
            "AIModule dependency is required by the civilian prototype.")
    require("\"NavigationSystem\"" in (ROOT / "Source/OpenWorldNextGen/OpenWorldNextGen.Build.cs").read_text(encoding="utf-8"),
            "NavigationSystem dependency is required by the civilian prototype.")

    save_header = (ROOT / "Source/OpenWorldNextGen/Persistence/OWSaveGame.h").read_text(encoding="utf-8")
    persistence_header = (ROOT / "Source/OpenWorldNextGen/Persistence/OWPersistenceSubsystem.h").read_text(encoding="utf-8")
    persistence_source = (ROOT / "Source/OpenWorldNextGen/Persistence/OWPersistenceSubsystem.cpp").read_text(encoding="utf-8")
    require("CurrentSchemaVersion = 1" in save_header and "UPROPERTY(VisibleAnywhere, SaveGame" in save_header,
            "SaveGame data must expose an explicit serialized schema.")
    for field in ("MapIdentifier", "PlayerTransform", "SelectedCharacterId", "SavedAtUtc"):
        require(field in save_header, f"Save schema is missing required field: {field}")
    for method in ("SaveCurrentGame", "LoadCurrentGame", "HasSaveGame"):
        require(method in persistence_header, f"Persistence subsystem API is missing: {method}")
    require("UGameInstanceSubsystem" in persistence_header and
            "CreateSaveGameObject" in persistence_source and "SaveGameToSlot" in persistence_source,
            "Persistence must use a dedicated game-instance service and Unreal SaveGame slots.")
    require("Unsupported save schema version" in persistence_source and
            "IsValidPlayerTransform" in persistence_source and "FMath::IsFinite" in persistence_source,
            "Save payload must reject unsupported schemas and non-finite transforms.")
    require("TryLoadValidatedSlot" in persistence_source and "BackupSlotSuffix" in persistence_source,
            "Save loading must validate primary and backup slots before applying state.")
    require("SetActorTransform" in persistence_source and
            persistence_source.find("TryLoadValidatedSlot(SaveSlotName + BackupSlotSuffix") <
            persistence_source.find("SetActorTransform"),
            "Player state must only be applied after validated primary/backup loading.")
    require("SaveGameToSlot(ExistingSave, BackupSlot" in persistence_source and
            "Cannot preserve the previous save" in persistence_source,
            "Replacing a valid primary save must preserve it in the backup slot first.")


    vehicle_header = (ROOT / "Source/OpenWorldNextGen/Vehicles/OWVehiclePawn.h").read_text(encoding="utf-8")
    vehicle_source = (ROOT / "Source/OpenWorldNextGen/Vehicles/OWVehiclePawn.cpp").read_text(encoding="utf-8")
    for token in ("IOWInteractable", "Interact_Implementation", "SetupPlayerInputComponent", "ResetVehicle"):
        require(token in vehicle_header, f"Vehicle pawn is missing required interface/API: {token}")
    for token in ("SetSimulatePhysics(true)", "SetMassOverrideInKg", "AddForce", "AddTorqueInRadians",
                  "FindTeleportSpot", "Possess(this)", "Possess(PawnToRestore)"):
        require(token in vehicle_source, f"Vehicle source is missing physics/possession safety behavior: {token}")
    require("PreviousPawn" in vehicle_header and "PreviousPawn.Reset()" in vehicle_source,
            "Vehicle must track and release the previous player pawn safely.")
    require("FMath::Clamp(Value, -1.0f, 1.0f)" in vehicle_source and
            "MaximumSpeedCmPerSecond" in vehicle_source,
            "Vehicle input and top-speed controls must be bounded.")
    vehicle_input = (ROOT / "Config/DefaultInput.ini").read_text(encoding="utf-8")
    for mapping in ("VehicleThrottle", "VehicleSteer", "VehicleBrake"):
        require(mapping in vehicle_input, f"Missing vehicle input mapping: {mapping}")
    require("ResetVehicle" in (ROOT / "Docs/VEHICLE_PROTOTYPE_SETUP.md").read_text(encoding="utf-8"),
            "Vehicle setup and recovery documentation is missing.")

    city_map_header = (ROOT / "Source/OpenWorldNextGen/World/OWCityMap.h").read_text(encoding="utf-8")
    require("GenerationSeed" in city_map_header and "FRandomStream RandomStream(GenerationSeed)" in city_map_source,
            "District generation must use an explicit reproducible seed.")
    require("DistrictCellCount" in city_map_header and "DistrictCellSize" in city_map_header,
            "District dimensions must be configurable from the editor.")
    require('ClampMin = "2", ClampMax = "12"' in city_map_header and 'ClampMin = "8000.0", ClampMax = "30000.0"' in city_map_header,
            "Editor-facing district dimensions must have conservative limits.")
    require("FMath::Clamp(DistrictCellCount, 2, 12)" in city_map_source and
            "FMath::Clamp(DistrictCellSize, 8000.0f, 30000.0f)" in city_map_source,
            "District dimensions must also be clamped at runtime.")
    require("RandomStream.RandRange(3, 5)" in city_map_source and "RandomStream.FRandRange" in city_map_source,
            "Building count and dimensions must use the seeded district random stream.")
    require("const float CellScale = CellSize / 18000.0f" in city_map_source and
            "const float RowSpacing = CellSize * 0.5f" in city_map_source,
            "Building placement and dimensions must scale with the configured cell size.")

    require("Component->SetCollisionEnabled(ECollisionEnabled::NoCollision)" in city_map_source,
            "Decorative props should not add unnecessary physics collision.")

    print("Static project validation passed.")
    print("Save/load schema and recovery source checks passed.")
    print("Civilian NavMesh wandering source checks passed.")
    print("PlayerStart initialization order checks passed.")
    print("Note: UnrealHeaderTool, MSVC compilation, editor launch and packaged-game tests were not run.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError, json.JSONDecodeError) as exc:
        print(f"Validation failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
