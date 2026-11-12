# Save/load setup and current limitations

## Implemented source boundary

The first persistence slice adds:
- UOWSaveGame, schema version 1, containing map identifier, player transform, selected-character ID and UTC timestamp;
- UOWPersistenceSubsystem, available through the game instance, with SaveCurrentGame, LoadCurrentGame, HasSaveGame, GetLoadedCharacterId and GetLastPersistenceError;
- primary slot OpenWorldNextGen_Save and backup slot OpenWorldNextGen_Save_Backup (platform save directory managed by Unreal's SaveGame system);
- validation of schema version, map identity, required character ID and finite/non-zero transform scale;
- validation before player transform mutation and backup fallback when the primary slot is missing, corrupt, incompatible or for another map.

Example C++ access:

    UOWPersistenceSubsystem* Persistence =
        GetGameInstance()->GetSubsystem<UOWPersistenceSubsystem>();

    if (Persistence)
    {
        Persistence->SaveCurrentGame(TEXT("Default"));
        // Later, on the same map:
        Persistence->LoadCurrentGame();
    }

The selected-character identifier is persisted as data only. The project does not yet have a three-character selection/switching system, so loading currently restores the player pawn transform and exposes the loaded ID through GetLoadedCharacterId().

## Recovery behavior

When replacing an existing primary save, the subsystem first deserializes and validates it, then writes it to the backup slot. If that backup write fails, the new primary save is not written. Loading validates the primary slot first and then attempts the backup slot. Invalid data is never applied to the active pawn.

Unreal's SaveGameToSlot API does not provide a general transaction guarantee for the underlying platform file write. The backup slot reduces the risk of losing the previous valid save but is not a substitute for platform-specific power-loss/atomic-write guarantees.

Save data is map-specific. Loading a save from another map is rejected rather than attempting an implicit travel.

## Required verification before closing issue #7

- Build with Unreal Engine 5.8 (UnrealHeaderTool + MSVC).
- Add Unreal Automation coverage for schema mismatch, invalid transforms, map mismatch, missing/corrupt primary slot, backup fallback and failed transform application.
- In a packaged Development build, save, exit, restart and load.
- Confirm actual save directory for the target platform and exercise recovery after replacing the primary slot with invalid data.

Static repository checks do not prove any of the above engine/runtime behavior. Issue #7 must remain open until the required build and packaged tests pass.
