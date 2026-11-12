#include "OWPersistenceSubsystem.h"

#include "OWSaveGame.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

const FString UOWPersistenceSubsystem::SaveSlotName = TEXT("OpenWorldNextGen_Save");
const FString UOWPersistenceSubsystem::BackupSlotSuffix = TEXT("_Backup");

namespace
{
    bool IsFiniteVector(const FVector& Value)
    {
        return FMath::IsFinite(Value.X) && FMath::IsFinite(Value.Y) && FMath::IsFinite(Value.Z);
    }

    bool IsFiniteQuaternion(const FQuat& Value)
    {
        return FMath::IsFinite(Value.X) && FMath::IsFinite(Value.Y) &&
               FMath::IsFinite(Value.Z) && FMath::IsFinite(Value.W);
    }

    bool IsValidPlayerTransform(const FTransform& Transform)
    {
        const FVector Scale = Transform.GetScale3D();
        return IsFiniteVector(Transform.GetLocation()) &&
               IsFiniteQuaternion(Transform.GetRotation()) &&
               IsFiniteVector(Scale) &&
               !FMath::IsNearlyZero(Scale.X) &&
               !FMath::IsNearlyZero(Scale.Y) &&
               !FMath::IsNearlyZero(Scale.Z);
    }
}

bool UOWPersistenceSubsystem::GetCurrentPlayerAndMap(
    UWorld*& OutWorld, APawn*& OutPawn, FString& OutMapIdentifier) const
{
    OutWorld = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    OutPawn = nullptr;
    OutMapIdentifier.Reset();

    if (!IsValid(OutWorld))
    {
        return false;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(OutWorld, 0);
    OutPawn = IsValid(PlayerController) ? PlayerController->GetPawn() : nullptr;
    OutMapIdentifier = UGameplayStatics::GetCurrentLevelName(OutWorld, true);

    return IsValid(OutPawn) && !OutMapIdentifier.IsEmpty();
}

bool UOWPersistenceSubsystem::ValidateSaveData(
    const UOWSaveGame* SaveData, const FString& ExpectedMap, FString& OutError) const
{
    if (!IsValid(SaveData))
    {
        OutError = TEXT("Save slot is missing or could not be deserialized.");
        return false;
    }

    if (SaveData->SchemaVersion != UOWSaveGame::CurrentSchemaVersion)
    {
        OutError = FString::Printf(TEXT("Unsupported save schema version: %d."),
                                   SaveData->SchemaVersion);
        return false;
    }

    if (SaveData->MapIdentifier.IsEmpty() ||
        (!ExpectedMap.IsEmpty() && SaveData->MapIdentifier != ExpectedMap))
    {
        OutError = TEXT("Save data references an empty or different map.");
        return false;
    }

    if (!IsValidPlayerTransform(SaveData->PlayerTransform))
    {
        OutError = TEXT("Save data contains an invalid player transform.");
        return false;
    }

    if (SaveData->SelectedCharacterId.IsNone())
    {
        OutError = TEXT("Save data does not contain a selected character identifier.");
        return false;
    }

    return true;
}

bool UOWPersistenceSubsystem::TryLoadValidatedSlot(
    const FString& SlotName, const FString& ExpectedMap,
    UOWSaveGame*& OutSaveData, FString& OutError) const
{
    OutSaveData = nullptr;
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, SaveUserIndex))
    {
        OutError = TEXT("Save slot does not exist.");
        return false;
    }

    UOWSaveGame* Candidate = Cast<UOWSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SlotName, SaveUserIndex));
    if (!ValidateSaveData(Candidate, ExpectedMap, OutError))
    {
        return false;
    }

    OutSaveData = Candidate;
    return true;
}

bool UOWPersistenceSubsystem::SaveCurrentGame(FName SelectedCharacterId)
{
    UWorld* World = nullptr;
    APawn* PlayerPawn = nullptr;
    FString MapIdentifier;
    if (!GetCurrentPlayerAndMap(World, PlayerPawn, MapIdentifier))
    {
        LastPersistenceError = TEXT("Cannot save: active world, player pawn, or map is unavailable.");
        return false;
    }

    const FTransform PlayerTransform = PlayerPawn->GetActorTransform();
    if (!IsValidPlayerTransform(PlayerTransform))
    {
        LastPersistenceError = TEXT("Cannot save: player transform is invalid.");
        return false;
    }

    UOWSaveGame* NewSave = Cast<UOWSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UOWSaveGame::StaticClass()));
    if (!IsValid(NewSave))
    {
        LastPersistenceError = TEXT("Cannot create save data object.");
        return false;
    }

    NewSave->SchemaVersion = UOWSaveGame::CurrentSchemaVersion;
    NewSave->MapIdentifier = MapIdentifier;
    NewSave->PlayerTransform = PlayerTransform;
    NewSave->SelectedCharacterId = SelectedCharacterId.IsNone()
        ? FName(TEXT("Default")) : SelectedCharacterId;
    NewSave->SavedAtUtc = FDateTime::UtcNow().ToIso8601();

    const FString BackupSlot = SaveSlotName + BackupSlotSuffix;
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        UOWSaveGame* ExistingSave = Cast<UOWSaveGame>(
            UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
        FString ExistingSaveError;
        if (ValidateSaveData(ExistingSave, FString(), ExistingSaveError))
        {
            if (!UGameplayStatics::SaveGameToSlot(ExistingSave, BackupSlot, SaveUserIndex))
            {
                LastPersistenceError = TEXT("Cannot preserve the previous save in the backup slot; primary save was not replaced.");
                return false;
            }
        }
    }

    if (!UGameplayStatics::SaveGameToSlot(NewSave, SaveSlotName, SaveUserIndex))
    {
        LastPersistenceError = TEXT("Writing the primary save slot failed. The previous backup, if present, was preserved.");
        return false;
    }

    LoadedCharacterId = NewSave->SelectedCharacterId;
    LastPersistenceError.Reset();
    return true;
}

bool UOWPersistenceSubsystem::LoadCurrentGame()
{
    UWorld* World = nullptr;
    APawn* PlayerPawn = nullptr;
    FString MapIdentifier;
    if (!GetCurrentPlayerAndMap(World, PlayerPawn, MapIdentifier))
    {
        LastPersistenceError = TEXT("Cannot load: active world, player pawn, or map is unavailable.");
        return false;
    }

    UOWSaveGame* SaveData = nullptr;
    FString PrimaryError;
    if (!TryLoadValidatedSlot(SaveSlotName, MapIdentifier, SaveData, PrimaryError))
    {
        FString BackupError;
        if (!TryLoadValidatedSlot(SaveSlotName + BackupSlotSuffix, MapIdentifier,
                                  SaveData, BackupError))
        {
            LastPersistenceError = FString::Printf(
                TEXT("Primary save rejected: %s Backup save rejected: %s"),
                *PrimaryError, *BackupError);
            return false;
        }
    }

    // No gameplay state is changed until a complete save payload has passed validation.
    if (!PlayerPawn->SetActorTransform(SaveData->PlayerTransform, false, nullptr,
                                       ETeleportType::TeleportPhysics))
    {
        LastPersistenceError = TEXT("The validated player transform could not be applied; world state was not accepted.");
        return false;
    }

    LoadedCharacterId = SaveData->SelectedCharacterId;
    LastPersistenceError.Reset();
    return true;
}

bool UOWPersistenceSubsystem::HasSaveGame() const
{
    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex) ||
           UGameplayStatics::DoesSaveGameExist(SaveSlotName + BackupSlotSuffix, SaveUserIndex);
}
