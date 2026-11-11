#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/OWInteractable.h"
#include "OWInteractableActor.generated.h"

UCLASS()
class OPENWORLDNEXTGEN_API AOWInteractableActor : public AActor, public IOWInteractable
{
    GENERATED_BODY()

public:
    AOWInteractableActor();
    virtual void Interact_Implementation(AActor* InteractingActor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    FText InteractionName;
};