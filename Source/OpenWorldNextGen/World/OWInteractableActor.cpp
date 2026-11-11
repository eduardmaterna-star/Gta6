#include "OWInteractableActor.h"

AOWInteractableActor::AOWInteractableActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    InteractionName = FText::FromString(TEXT("Interactable"));
}

void AOWInteractableActor::Interact_Implementation(AActor* InteractingActor)
{
    if (!IsValid(InteractingActor))
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("%s interacted with %s (%s)"),
        *GetNameSafe(InteractingActor), *GetName(), *InteractionName.ToString());
}