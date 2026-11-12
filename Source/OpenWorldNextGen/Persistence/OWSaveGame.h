#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OWSaveGame.generated.h"

/** Versioned, actor-pointer-free data for the first single-player save milestone. */
UCLASS()
class OPENWORLDNEXTGEN_API UOWSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    static constexpr int32 CurrentSchemaVersion = 1;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save")
    int32 SchemaVersion = CurrentSchemaVersion;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save")
    FString MapIdentifier;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save")
    FTransform PlayerTransform = FTransform::Identity;

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save")
    FName SelectedCharacterId = TEXT("Default");

    UPROPERTY(VisibleAnywhere, SaveGame, Category = "Save")
    FString SavedAtUtc;
};
