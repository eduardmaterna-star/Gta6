#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OWCivilianPopulationSpawner.generated.h"

class AOWCivilianCharacter;

/**
 * Bounded, timer-driven owner for a small civilian test population.
 * Place one instance in a test level with a built NavMesh.
 */
UCLASS()
class OPENWORLDNEXTGEN_API AOWCivilianPopulationSpawner : public AActor
{
    GENERATED_BODY()

public:
    AOWCivilianPopulationSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SpawnNextCivilian();
    void ScheduleNextSpawn(float DelaySeconds);
    void PruneInvalidCivilians();
    bool TryFindReachableSpawnLocation(FVector& OutLocation);

    UPROPERTY(EditAnywhere, Category = "Civilian Population")
    TSubclassOf<AOWCivilianCharacter> CivilianClass;

    UPROPERTY(EditAnywhere, Category = "Civilian Population", meta = (ClampMin = "0", ClampMax = "50"))
    int32 MaxPopulation = 12;

    UPROPERTY(EditAnywhere, Category = "Civilian Population", meta = (ClampMin = "500.0", ClampMax = "10000.0"))
    float SpawnRadius = 2500.0f;

    UPROPERTY(EditAnywhere, Category = "Civilian Population", meta = (ClampMin = "0.25", ClampMax = "10.0"))
    float SpawnInterval = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Civilian Population", meta = (ClampMin = "1.0", ClampMax = "30.0"))
    float RetryInterval = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Civilian Population", meta = (ClampMin = "1", ClampMax = "12"))
    int32 MaxLocationAttemptsPerSpawn = 6;

    UPROPERTY(EditAnywhere, Category = "Civilian Population")
    int32 RandomSeed = 1337;

    FRandomStream RandomStream;
    FTimerHandle SpawnTimerHandle;
    TArray<TWeakObjectPtr<AOWCivilianCharacter>> SpawnedCivilians;
    bool bWarnedMissingNavigation = false;
};
