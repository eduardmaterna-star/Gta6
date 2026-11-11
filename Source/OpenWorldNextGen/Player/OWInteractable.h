#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OWInteractable.generated.h"

UINTERFACE(BlueprintType)
class OPENWORLDNEXTGEN_API UOWInteractable : public UInterface
{
    GENERATED_BODY()
};

/** Implement on actors the player can interact with. Blueprint implementations are supported. */
class OPENWORLDNEXTGEN_API IOWInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* InteractingActor);
};