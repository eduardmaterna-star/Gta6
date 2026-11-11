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
    require("PrimaryActorTick.bCanEverTick = false" in civilian_source,
            "Civilian prototype should remain timer-driven without an unnecessary per-frame tick.")
    require("ClearTimer(WanderTimerHandle)" in civilian_source,
            "Civilian timer must be cleared on EndPlay.")

    require("\"AIModule\"" in (ROOT / "Source/OpenWorldNextGen/OpenWorldNextGen.Build.cs").read_text(encoding="utf-8"),
            "AIModule dependency is required by the civilian prototype.")
    require("\"NavigationSystem\"" in (ROOT / "Source/OpenWorldNextGen/OpenWorldNextGen.Build.cs").read_text(encoding="utf-8"),
            "NavigationSystem dependency is required by the civilian prototype.")

    require("Component->SetCollisionEnabled(ECollisionEnabled::NoCollision)" in city_map_source,
            "Decorative props should not add unnecessary physics collision.")

    print("Static project validation passed.")
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
