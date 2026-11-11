#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OWGameMode.generated.h"

UCLASS()
class OPENWORLDNEXTGEN_API AOWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AOWGameMode();

protected:
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void BeginPlay() override;
};