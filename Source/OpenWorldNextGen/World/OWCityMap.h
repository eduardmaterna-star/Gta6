#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OWCityMap.generated.h"

class UInstancedStaticMeshComponent;

/** Runtime-generated greybox district used as the first playable map milestone. */
UCLASS()
class OPENWORLDNEXTGEN_API AOWCityMap : public AActor
{
    GENERATED_BODY()

public:
    AOWCityMap();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> Ground;

    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> Roads;

    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> Sidewalks;

    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> Buildings;

    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> StreetFurniture;

    UPROPERTY(VisibleAnywhere, Category = "Map")
    TObjectPtr<UInstancedStaticMeshComponent> ParkProps;

    // Stable seed keeps generated blockout layouts reproducible across runs.
    UPROPERTY(EditDefaultsOnly, Category = "Map|Generation")
    int32 GenerationSeed = 20261111;

    // Keep runtime greybox generation bounded until the map is migrated to streamed editor-authored cells.
    UPROPERTY(EditDefaultsOnly, Category = "Map|Generation", meta = (ClampMin = "2", ClampMax = "12"))
    int32 DistrictCellCount = 6;

    UPROPERTY(EditDefaultsOnly, Category = "Map|Generation", meta = (ClampMin = "8000.0", ClampMax = "30000.0", Units = "cm"))
    float DistrictCellSize = 18000.0f;

    void BuildDistrict();
    void AddBox(UInstancedStaticMeshComponent* Component, const FVector& Center, const FVector& Size) const;
};