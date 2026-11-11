#include "OWGameMode.h"

#include "Player/OWPlayerCharacter.h"
#include "World/OWCityMap.h"
#include "Engine/World.h"

AOWGameMode::AOWGameMode()
{
    DefaultPawnClass = AOWPlayerCharacter::StaticClass();
}

void AOWGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (GetWorld())
    {
        GetWorld()->SpawnActor<AOWCityMap>(AOWCityMap::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    }
}