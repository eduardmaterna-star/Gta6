#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OWPersistenceSubsystem.generated.h"

class UOWSaveGame;

/** Owns save-slot I/O and validates a complete payload before mutating the player. */
UCLASS()
class OPENWORLDNEXTGEN_API UOWPersistenceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool SaveCurrentGame(FName SelectedCharacterId = TEXT("Default"));

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    bool LoadCurrentGame();

    UFUNCTION(BlueprintPure, Category = "Persistence")
    bool HasSaveGame() const;

    UFUNCTION(BlueprintPure, Category = "Persistence")
    FName GetLoadedCharacterId() const { return LoadedCharacterId; }

    UFUNCTION(BlueprintPure, Category = "Persistence")
    FString GetLastPersistenceError() const { return LastPersistenceError; }

private:
    static constexpr int32 SaveUserIndex = 0;
    static const FString SaveSlotName;
    static const FString BackupSlotSuffix;

    bool ValidateSaveData(const UOWSaveGame* SaveData, const FString& ExpectedMap,
                          FString& OutError) const;
    bool TryLoadValidatedSlot(const FString& SlotName, const FString& ExpectedMap,
                              UOWSaveGame*& OutSaveData, FString& OutError) const;
    bool GetCurrentPlayerAndMap(class UWorld*& OutWorld, class APawn*& OutPawn,
                                FString& OutMapIdentifier) const;

    UPROPERTY(Transient)
    FName LoadedCharacterId = TEXT("Default");

    UPROPERTY(Transient)
    FString LastPersistenceError;
};
