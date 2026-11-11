#include "OWCivilianCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"

AOWCivilianCharacter::AOWCivilianCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    GetCharacterMovement()->MaxWalkSpeed = 220.0f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
    GetCharacterMovement()->MaxAcceleration = 600.0f;
    // Enable Unreal's built-in RVO steering as a first layer of local NPC avoidance.
    // This is not a replacement for a full crowd manager or traffic simulation.
    GetCharacterMovement()->bUseRVOAvoidance = true;
    GetCharacterMovement()->AvoidanceConsiderationRadius = 400.0f;
    GetCharacterMovement()->AvoidanceWeight = 0.5f;

    // Temporary proxy makes placed test civilians visible before character art exists.
    PlaceholderBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBody"));
    PlaceholderBody->SetupAttachment(GetCapsuleComponent());
    PlaceholderBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PlaceholderBody->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
    PlaceholderBody->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.6f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (PlaceholderMesh.Succeeded())
    {
        PlaceholderBody->SetStaticMesh(PlaceholderMesh.Object);
    }
}

void AOWCivilianCharacter::BeginPlay()
{
    Super::BeginPlay();
    ScheduleNextAttempt(FMath::FRandRange(0.5f, 2.0f));
}

void AOWCivilianCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(WanderTimerHandle);

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
    }

    Super::EndPlay(EndPlayReason);
}

void AOWCivilianCharacter::ScheduleNextAttempt(float DelaySeconds)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            WanderTimerHandle,
            this,
            &AOWCivilianCharacter::ChooseNextWanderDestination,
            FMath::Max(0.1f, DelaySeconds),
            false);
    }
}

void AOWCivilianCharacter::ChooseNextWanderDestination()
{
    UWorld* World = GetWorld();
    AAIController* AIController = Cast<AAIController>(GetController());
    UNavigationSystemV1* NavigationSystem = World ? UNavigationSystemV1::GetCurrent(World) : nullptr;

    if (!World || !AIController || !NavigationSystem)
    {
        // Retry later: AI possession or navigation data may not be ready yet.
        ScheduleNextAttempt(2.0f);
        return;
    }

    // Do not replace a path that is still being followed. Long routes may take
    // longer than WanderInterval, especially when the NavMesh is obstructed.
    if (AIController->GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        ScheduleNextAttempt(1.0f);
        return;
    }

    FNavLocation Destination;
    const bool bFoundDestination = NavigationSystem->GetRandomReachablePointInRadius(
        GetActorLocation(),
        WanderRadius,
        Destination);

    if (!bFoundDestination)
    {
        // A missing NavMesh should not cause a tight retry loop or a crash.
        ScheduleNextAttempt(3.0f);
        return;
    }

    const float EffectiveMinimumDistance = FMath::Min(MinimumWanderDistance, WanderRadius * 0.8f);
    if (FVector::DistSquared2D(GetActorLocation(), Destination.Location)
        < FMath::Square(EffectiveMinimumDistance))
    {
        // Avoid selecting a point so close that the civilian appears stationary.
        ScheduleNextAttempt(1.5f);
        return;
    }

    const EPathFollowingRequestResult::Type RequestResult =
        AIController->MoveToLocation(Destination.Location, AcceptanceRadius);

    if (RequestResult == EPathFollowingRequestResult::Failed)
    {
        ScheduleNextAttempt(2.0f);
        return;
    }

    ScheduleNextAttempt(FMath::FRandRange(WanderInterval * 0.75f, WanderInterval * 1.25f));
}
