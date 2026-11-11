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

    void BuildDistrict();
    void AddBox(UInstancedStaticMeshComponent* Component, const FVector& Center, const FVector& Size) const;
};