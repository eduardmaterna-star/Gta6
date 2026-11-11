#include "OWPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "OWInteractable.h"

AOWPlayerCharacter::AOWPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.25f;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AOWPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AOWPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AOWPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AOWPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AOWPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AOWPlayerCharacter::LookUp);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AOWPlayerCharacter::Interact);
}

void AOWPlayerCharacter::MoveForward(float Value)
{
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void AOWPlayerCharacter::MoveRight(float Value)
{
    if (Controller && !FMath::IsNearlyZero(Value))
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void AOWPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AOWPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AOWPlayerCharacter::Interact()
{
    if (!FollowCamera || !GetWorld())
    {
        return;
    }

    const FVector Start = FollowCamera->GetComponentLocation();
    const FVector End = Start + FollowCamera->GetForwardVector() * InteractionDistance;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerInteraction), false, this);
    FHitResult Hit;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
    {
        AActor* HitActor = Hit.GetActor();
        if (IsValid(HitActor) && HitActor->GetClass()->ImplementsInterface(UOWInteractable::StaticClass()))
        {
            IOWInteractable::Execute_Interact(HitActor, this);
        }
    }
}