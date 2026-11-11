#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OWPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class OPENWORLDNEXTGEN_API AOWPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AOWPlayerCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void Interact();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
    float InteractionDistance = 300.0f;
};