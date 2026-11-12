#include "OWGameMode.h"

#include "Player/OWPlayerCharacter.h"
#include "World/OWCityMap.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

AOWGameMode::AOWGameMode()
{
    DefaultPawnClass = AOWPlayerCharacter::StaticClass();
}

void AOWGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("OWGameMode: world is unavailable during initialization; player start was not created."));
        return;
    }

    // Reuse a PlayerStart authored in the level; create a fallback before players are spawned otherwise.
    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        return;
    }

    const FTransform StartTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, 250.0f));
    if (!World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), StartTransform))
    {
        UE_LOG(LogTemp, Error, TEXT("OWGameMode: failed to create the fallback player start."));
    }
}

void AOWGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("OWGameMode: world is unavailable; city map was not spawned."));
        return;
    }

    // Allow an editor-authored city map to take precedence and prevent duplicate runtime geometry.
    for (TActorIterator<AOWCityMap> It(World); It; ++It)
    {
        return;
    }

    if (!World->SpawnActor<AOWCityMap>(AOWCityMap::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator))
    {
        UE_LOG(LogTemp, Error, TEXT("OWGameMode: failed to spawn the fallback city map."));
    }
}
