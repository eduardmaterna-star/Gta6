#include "OWVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

AOWVehiclePawn::AOWVehiclePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Disabled;

    Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
    SetRootComponent(Chassis);
    Chassis->SetCollisionProfileName(TEXT("PhysicsActor"));
    Chassis->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Chassis->SetSimulatePhysics(true);
    Chassis->SetEnableGravity(true);
    Chassis->SetLinearDamping(0.12f);
    Chassis->SetAngularDamping(2.5f);
    Chassis->SetMassOverrideInKg(NAME_None, 1400.0f, true);
    Chassis->SetNotifyRigidBodyCollision(true);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ChassisMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (ChassisMesh.Succeeded())
    {
        Chassis->SetStaticMesh(ChassisMesh.Object);
        Chassis->SetRelativeScale3D(FVector(2.2f, 1.0f, 0.55f));
    }

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(Chassis);
    CameraBoom->TargetArmLength = 650.0f;
    CameraBoom->SetRelativeLocation(FVector(-100.0f, 0.0f, 180.0f));
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 8.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AOWVehiclePawn::BeginPlay()
{
    Super::BeginPlay();
    if (!Chassis || !Chassis->GetStaticMesh())
    {
        UE_LOG(LogTemp, Error, TEXT("OWVehiclePawn: chassis mesh is unavailable; vehicle cannot be driven."));
        SetActorTickEnabled(false);
    }
}

void AOWVehiclePawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!Chassis || !Chassis->IsSimulatingPhysics() || DeltaSeconds <= 0.0f)
    {
        return;
    }

    const FVector Velocity = Chassis->GetPhysicsLinearVelocity();
    const FVector Forward = Chassis->GetForwardVector();
    const FVector Right = Chassis->GetRightVector();
    const float ForwardSpeed = FVector::DotProduct(Velocity, Forward);

    if (!FMath::IsNearlyZero(ThrottleInput) &&
        Velocity.SizeSquared() < FMath::Square(MaximumSpeedCmPerSecond))
    {
        Chassis->AddForce(Forward * (ThrottleInput * MaximumDriveForce));
    }

    const float LateralSpeed = FVector::DotProduct(Velocity, Right);
    const float LateralForce = FMath::Clamp(
        -LateralSpeed * 1400.0f, -LateralGripForce, LateralGripForce);
    Chassis->AddForce(Right * LateralForce);

    if (bBrakePressed && !Velocity.IsNearlyZero())
    {
        const float BrakeMagnitude = FMath::Min(MaximumBrakeForce, Velocity.Size() * 12000.0f);
        Chassis->AddForce(-Velocity.GetSafeNormal() * BrakeMagnitude);
    }

    const float SpeedFactor = FMath::Clamp(FMath::Abs(ForwardSpeed) / 500.0f, 0.0f, 1.0f);
    if (!FMath::IsNearlyZero(SteeringInput) && SpeedFactor > 0.02f)
    {
        const float ReverseSign = ForwardSpeed < -10.0f ? -1.0f : 1.0f;
        Chassis->AddTorqueInRadians(
            Chassis->GetUpVector() * SteeringInput * MaximumSteeringTorque * SpeedFactor * ReverseSign);
    }
}

void AOWVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("VehicleThrottle"), this, &AOWVehiclePawn::SetThrottle);
    PlayerInputComponent->BindAxis(TEXT("VehicleSteer"), this, &AOWVehiclePawn::SetSteering);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Pressed, this, &AOWVehiclePawn::SetBrakePressed);
    PlayerInputComponent->BindAction(TEXT("VehicleBrake"), IE_Released, this, &AOWVehiclePawn::SetBrakeReleased);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AOWVehiclePawn::ExitVehicle);
}

void AOWVehiclePawn::Interact_Implementation(AActor* InteractingActor)
{
    APawn* EnteringPawn = Cast<APawn>(InteractingActor);
    APlayerController* PlayerController = EnteringPawn
        ? Cast<APlayerController>(EnteringPawn->GetController()) : nullptr;

    if (!IsValid(EnteringPawn) || !IsValid(PlayerController) ||
        !Chassis || !Chassis->IsSimulatingPhysics() || PlayerController->GetPawn() != EnteringPawn)
    {
        return;
    }

    PreviousPawn = EnteringPawn;
    ThrottleInput = 0.0f;
    SteeringInput = 0.0f;
    bBrakePressed = false;
    PlayerController->Possess(this);
}

void AOWVehiclePawn::ExitVehicle()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    APawn* PawnToRestore = PreviousPawn.Get();
    if (!IsValid(PlayerController) || !IsValid(PawnToRestore) || !Chassis)
    {
        return;
    }

    FVector ExitLocation = GetActorLocation() + GetActorRightVector() * ExitOffset;
    FRotator ExitRotation = PawnToRestore->GetActorRotation();
    if (!GetWorld() || !GetWorld()->FindTeleportSpot(PawnToRestore, ExitLocation, ExitRotation))
    {
        return;
    }

    PawnToRestore->SetActorLocationAndRotation(ExitLocation, ExitRotation, false, nullptr,
                                                ETeleportType::TeleportPhysics);
    PlayerController->Possess(PawnToRestore);
    PreviousPawn.Reset();
    ThrottleInput = 0.0f;
    SteeringInput = 0.0f;
    bBrakePressed = false;
}

void AOWVehiclePawn::SetThrottle(float Value)
{
    ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AOWVehiclePawn::SetSteering(float Value)
{
    SteeringInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AOWVehiclePawn::SetBrakePressed()
{
    bBrakePressed = true;
}

void AOWVehiclePawn::SetBrakeReleased()
{
    bBrakePressed = false;
}

bool AOWVehiclePawn::ResetVehicle()
{
    if (!Chassis || !Chassis->IsSimulatingPhysics() || !GetWorld())
    {
        return false;
    }

    const FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 120.0f);
    const FRotator ResetRotation(0.0f, GetActorRotation().Yaw, 0.0f);
    if (!SetActorLocationAndRotation(ResetLocation, ResetRotation, true, nullptr,
                                     ETeleportType::TeleportPhysics))
    {
        return false;
    }

    Chassis->SetPhysicsLinearVelocity(FVector::ZeroVector);
    Chassis->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    ThrottleInput = 0.0f;
    SteeringInput = 0.0f;
    bBrakePressed = false;
    return true;
}

void AOWVehiclePawn::UnPossessed()
{
    ThrottleInput = 0.0f;
    SteeringInput = 0.0f;
    bBrakePressed = false;
    Super::UnPossessed();
}
