#include "OWCivilianPopulationSpawner.h"

#include "AI/OWCivilianCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

AOWCivilianPopulationSpawner::AOWCivilianPopulationSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    CivilianClass = AOWCivilianCharacter::StaticClass();
}

void AOWCivilianPopulationSpawner::BeginPlay()
{
    Super::BeginPlay();

    RandomStream.Initialize(RandomSeed);
    if (MaxPopulation <= 0)
    {
        return;
    }

    ScheduleNextSpawn(0.25f);
}

void AOWCivilianPopulationSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }

    // This spawner owns its test population; do not leave orphaned actors behind.
    for (const TWeakObjectPtr<AOWCivilianCharacter>& CivilianPtr : SpawnedCivilians)
    {
        if (AOWCivilianCharacter* Civilian = CivilianPtr.Get())
        {
            Civilian->Destroy();
        }
    }
    SpawnedCivilians.Reset();

    Super::EndPlay(EndPlayReason);
}

void AOWCivilianPopulationSpawner::ScheduleNextSpawn(float DelaySeconds)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            SpawnTimerHandle,
            this,
            &AOWCivilianPopulationSpawner::SpawnNextCivilian,
            FMath::Max(0.1f, DelaySeconds),
            false);
    }
}

void AOWCivilianPopulationSpawner::PruneInvalidCivilians()
{
    SpawnedCivilians.RemoveAll([](const TWeakObjectPtr<AOWCivilianCharacter>& CivilianPtr)
    {
        return !CivilianPtr.IsValid();
    });
}

bool AOWCivilianPopulationSpawner::TryFindReachableSpawnLocation(FVector& OutLocation)
{
    UWorld* World = GetWorld();
    UNavigationSystemV1* NavigationSystem = World ? UNavigationSystemV1::GetCurrent(World) : nullptr;
    if (!World || !NavigationSystem)
    {
        return false;
    }

    const float SafeRadius = FMath::Clamp(SpawnRadius, 500.0f, 10000.0f);
    const int32 Attempts = FMath::Clamp(MaxLocationAttemptsPerSpawn, 1, 12);
    const FVector Origin = GetActorLocation();

    for (int32 Attempt = 0; Attempt < Attempts; ++Attempt)
    {
        // Seeded disk sampling makes candidate positions repeatable for a fixed seed.
        const float Angle = RandomStream.FRandRange(0.0f, 2.0f * PI);
        const float Distance = SafeRadius * FMath::Sqrt(RandomStream.FRand());
        const FVector Candidate = Origin + FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.0f);

        FNavLocation ProjectedLocation;
        if (!NavigationSystem->ProjectPointToNavigation(
                Candidate,
                ProjectedLocation,
                FVector(150.0f, 150.0f, 500.0f)))
        {
            continue;
        }

        // Projection alone can land on a disconnected NavMesh island. Confirm a path
        // from the spawner before attempting to create the character.
        double PathLength = 0.0;
        const ENavigationQueryResult::Type PathResult = UNavigationSystemV1::GetPathLength(
            World,
            Origin,
            ProjectedLocation.Location,
            PathLength,
            nullptr,
            nullptr);

        if (PathResult != ENavigationQueryResult::Success || !FMath::IsFinite(PathLength))
        {
            continue;
        }

        OutLocation = ProjectedLocation.Location;
        return true;
    }

    return false;
}

void AOWCivilianPopulationSpawner::SpawnNextCivilian()
{
    PruneInvalidCivilians();

    if (SpawnedCivilians.Num() >= FMath::Clamp(MaxPopulation, 0, 50))
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World || !CivilianClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Civilian population spawner: world or civilian class is unavailable."));
        ScheduleNextSpawn(RetryInterval);
        return;
    }

    if (!UNavigationSystemV1::GetCurrent(World))
    {
        if (!bWarnedMissingNavigation)
        {
            UE_LOG(LogTemp, Warning, TEXT("Civilian population spawner: no NavigationSystem is available; retrying with a delay."));
            bWarnedMissingNavigation = true;
        }
        ScheduleNextSpawn(RetryInterval);
        return;
    }

    bWarnedMissingNavigation = false;

    FVector NavLocation = FVector::ZeroVector;
    if (!TryFindReachableSpawnLocation(NavLocation))
    {
        // Bounded retry: a missing or incomplete NavMesh never creates a tight loop.
        ScheduleNextSpawn(RetryInterval);
        return;
    }

    const AOWCivilianCharacter* CivilianDefault = CivilianClass->GetDefaultObject<AOWCivilianCharacter>();
    const float CapsuleHalfHeight = CivilianDefault && CivilianDefault->GetCapsuleComponent()
        ? CivilianDefault->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
        : 96.0f;
    const FVector SpawnLocation = NavLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight);

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

    AOWCivilianCharacter* SpawnedCivilian = World->SpawnActor<AOWCivilianCharacter>(
        CivilianClass,
        SpawnLocation,
        FRotator(0.0f, RandomStream.FRandRange(-180.0f, 180.0f), 0.0f),
        SpawnParameters);

    if (!SpawnedCivilian)
    {
        ScheduleNextSpawn(RetryInterval);
        return;
    }

    SpawnedCivilians.Add(SpawnedCivilian);

    // Spawn gradually to avoid a single-frame burst of actor creation and path queries.
    if (SpawnedCivilians.Num() < FMath::Clamp(MaxPopulation, 0, 50))
    {
        ScheduleNextSpawn(SpawnInterval);
    }
}
