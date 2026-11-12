#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player/OWInteractable.h"
#include "OWVehiclePawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;

/** First drivable arcade-physics vehicle slice; not a wheel-based Chaos Vehicles implementation. */
UCLASS()
class OPENWORLDNEXTGEN_API AOWVehiclePawn : public APawn, public IOWInteractable
{
    GENERATED_BODY()

public:
    AOWVehiclePawn();
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Interact_Implementation(AActor* InteractingActor) override;

    UFUNCTION(BlueprintCallable, Category = "Vehicle")
    bool ResetVehicle();

protected:
    virtual void BeginPlay() override;
    virtual void UnPossessed() override;

private:
    void SetThrottle(float Value);
    void SetSteering(float Value);
    void SetBrakePressed();
    void SetBrakeReleased();
    void ExitVehicle();

    UPROPERTY(VisibleAnywhere, Category = "Vehicle")
    TObjectPtr<UStaticMeshComponent> Chassis;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Handling", meta = (ClampMin = "10000.0", ClampMax = "2000000.0"))
    float MaximumDriveForce = 900000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Handling", meta = (ClampMin = "10000.0", ClampMax = "2000000.0"))
    float MaximumBrakeForce = 650000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Handling", meta = (ClampMin = "1000.0", ClampMax = "500000.0"))
    float LateralGripForce = 120000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Handling", meta = (ClampMin = "10000.0", ClampMax = "2000000.0"))
    float MaximumSteeringTorque = 350000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Handling", meta = (ClampMin = "0.0", ClampMax = "5000.0"))
    float MaximumSpeedCmPerSecond = 4500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Vehicle|Recovery", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
    float ExitOffset = 250.0f;

    UPROPERTY(Transient)
    TWeakObjectPtr<APawn> PreviousPawn;

    float ThrottleInput = 0.0f;
    float SteeringInput = 0.0f;
    bool bBrakePressed = false;
};
