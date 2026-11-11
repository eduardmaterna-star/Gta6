#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OWCivilianCharacter.generated.h"

class UStaticMeshComponent;

/**
 * Lightweight civilian navigation prototype.
 *
 * Requires a NavMesh (NavMeshBoundsVolume) in the level. This is a movement
 * foundation only; perception, schedules, dialogue and crowd simulation are
 * intentionally outside this first implementation.
 */
UCLASS()
class OPENWORLDNEXTGEN_API AOWCivilianCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOWCivilianCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void ChooseNextWanderDestination();
    void ScheduleNextAttempt(float DelaySeconds);

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    TObjectPtr<UStaticMeshComponent> PlaceholderBody;

    UPROPERTY(EditDefaultsOnly, Category = "Civilian|Movement", meta = (ClampMin = "300.0", ClampMax = "10000.0"))
    float WanderRadius = 1800.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Civilian|Movement", meta = (ClampMin = "1.0", ClampMax = "60.0"))
    float WanderInterval = 8.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Civilian|Movement", meta = (ClampMin = "10.0", ClampMax = "300.0"))
    float AcceptanceRadius = 80.0f;

    FTimerHandle WanderTimerHandle;
};
