#include "OWCityMap.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    constexpr float RoadWidth = 1400.0f;
}

AOWCityMap::AOWCityMap()
{
    PrimaryActorTick.bCanEverTick = false;

    Ground = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Ground"));
    SetRootComponent(Ground);
    Roads = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Roads"));
    Roads->SetupAttachment(RootComponent);
    Sidewalks = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Sidewalks"));
    Sidewalks->SetupAttachment(RootComponent);
    Buildings = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Buildings"));
    Buildings->SetupAttachment(RootComponent);
    StreetFurniture = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StreetFurniture"));
    StreetFurniture->SetupAttachment(RootComponent);
    ParkProps = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ParkProps"));
    ParkProps->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        Ground->SetStaticMesh(CubeMesh.Object);
        Roads->SetStaticMesh(CubeMesh.Object);
        Sidewalks->SetStaticMesh(CubeMesh.Object);
        Buildings->SetStaticMesh(CubeMesh.Object);
        StreetFurniture->SetStaticMesh(CubeMesh.Object);
        ParkProps->SetStaticMesh(CubeMesh.Object);
    }

    // Only world geometry that the player must stand on or collide with participates in physics.
    for (UInstancedStaticMeshComponent* Component : { Ground.Get(), Roads.Get(), Sidewalks.Get(), Buildings.Get() })
    {
        Component->SetMobility(EComponentMobility::Static);
        Component->SetCollisionProfileName(TEXT("BlockAll"));
        Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Component->SetGenerateOverlapEvents(false);
    }

    // Decorative props are batched visually but do not need per-instance physics collision.
    for (UInstancedStaticMeshComponent* Component : { StreetFurniture.Get(), ParkProps.Get() })
    {
        Component->SetMobility(EComponentMobility::Static);
        Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Component->SetGenerateOverlapEvents(false);
    }
}

void AOWCityMap::BeginPlay()
{
    Super::BeginPlay();
    BuildDistrict();
}

void AOWCityMap::AddBox(UInstancedStaticMeshComponent* Component, const FVector& Center, const FVector& Size) const
{
    if (!Component || Size.X <= 0.0f || Size.Y <= 0.0f || Size.Z <= 0.0f)
    {
        return;
    }

    const FVector Scale(Size.X / 100.0f, Size.Y / 100.0f, Size.Z / 100.0f);
    Component->AddInstance(FTransform(FRotator::ZeroRotator, Center, Scale));
}

void AOWCityMap::BuildDistrict()
{
    // Clamp again at runtime: editor metadata improves UX but is not a runtime safety boundary.
    const int32 CellCount = FMath::Clamp(DistrictCellCount, 2, 12);
    const float CellSize = FMath::Clamp(DistrictCellSize, 8000.0f, 30000.0f);

    if (!Ground || !Roads || !Sidewalks || !Buildings || !StreetFurniture || !ParkProps || !Ground->GetStaticMesh())
    {
        UE_LOG(LogTemp, Error, TEXT("OWCityMap: basic cube mesh is unavailable; district generation skipped."));
        return;
    }

    AddBox(Ground, FVector(0.0f, 0.0f, -80.0f), FVector(CellCount * CellSize + 5000.0f, CellCount * CellSize + 5000.0f, 100.0f));

    for (int32 RoadIndex = 0; RoadIndex <= CellCount; ++RoadIndex)
    {
        const float Coordinate = (RoadIndex - CellCount * 0.5f) * CellSize;
        AddBox(Roads, FVector(Coordinate, 0.0f, -15.0f), FVector(RoadWidth, CellCount * CellSize + RoadWidth, 30.0f));
        AddBox(Roads, FVector(0.0f, Coordinate, -15.0f), FVector(CellCount * CellSize + RoadWidth, RoadWidth, 30.0f));

        const float SidewalkOffset = RoadWidth * 0.5f + 180.0f;
        for (float Side : { -1.0f, 1.0f })
        {
            AddBox(Sidewalks, FVector(Coordinate + Side * SidewalkOffset, 0.0f, 10.0f), FVector(360.0f, CellCount * CellSize, 20.0f));
            AddBox(Sidewalks, FVector(0.0f, Coordinate + Side * SidewalkOffset, 10.0f), FVector(CellCount * CellSize, 360.0f, 20.0f));
        }
    }

    const int32 ParkCell = CellCount / 2;
    const float ParkX = (ParkCell - CellCount * 0.5f + 0.5f) * CellSize;
    const float ParkY = ParkX;
    const float ParkExtent = CellSize - RoadWidth - 900.0f;
    AddBox(Ground, FVector(ParkX, ParkY, 15.0f), FVector(ParkExtent, ParkExtent, 30.0f));

    for (int32 TreeIndex = 0; TreeIndex < 16; ++TreeIndex)
    {
        const float Angle = (2.0f * PI * TreeIndex) / 16.0f;
        const float Radius = ParkExtent * 0.34f;
        const FVector TreeLocation(ParkX + FMath::Cos(Angle) * Radius, ParkY + FMath::Sin(Angle) * Radius, 0.0f);
        AddBox(ParkProps, TreeLocation + FVector(0.0f, 0.0f, 180.0f), FVector(45.0f, 45.0f, 360.0f));
        AddBox(ParkProps, TreeLocation + FVector(0.0f, 0.0f, 420.0f), FVector(260.0f, 260.0f, 260.0f));
    }

    // A single seeded stream makes the same seed produce the same district layout.
    FRandomStream RandomStream(GenerationSeed);

    for (int32 X = 0; X < CellCount; ++X)
    {
        for (int32 Y = 0; Y < CellCount; ++Y)
        {
            if (X == ParkCell && Y == ParkCell)
            {
                continue;
            }

            const float BlockCenterX = (X - CellCount * 0.5f + 0.5f) * CellSize;
            const float BlockCenterY = (Y - CellCount * 0.5f + 0.5f) * CellSize;
            const int32 BuildingCount = RandomStream.RandRange(3, 5);

            for (int32 BuildingIndex = 0; BuildingIndex < BuildingCount; ++BuildingIndex)
            {
                const float CellScale = CellSize / 18000.0f;
                const float Width = RandomStream.FRandRange(2200.0f, 3500.0f) * CellScale;
                const float Depth = RandomStream.FRandRange(2200.0f, 3500.0f) * CellScale;
                const float Height = RandomStream.FRandRange(900.0f, 4800.0f);
                const int32 RowCount = (BuildingCount + 1) / 2;
                const int32 RowIndex = BuildingIndex / 2;
                const float RowSpacing = CellSize * 0.5f / FMath::Max(1, RowCount - 1);
                const float LocalX = ((BuildingIndex % 2) == 0 ? -1.0f : 1.0f) * CellSize * 0.22f;
                const float LocalY = (RowIndex - (RowCount - 1) * 0.5f) * RowSpacing;
                AddBox(Buildings, FVector(BlockCenterX + LocalX, BlockCenterY + LocalY, Height * 0.5f + 70.0f), FVector(Width, Depth, Height));
            }
        }
    }

    for (int32 Index = 0; Index <= CellCount; ++Index)
    {
        const float Coordinate = (Index - CellCount * 0.5f) * CellSize;
        for (int32 Segment = 0; Segment < CellCount; ++Segment)
        {
            const float Along = (Segment - CellCount * 0.5f + 0.5f) * CellSize;
            for (float Side : { -1.0f, 1.0f })
            {
                const FVector Position(Coordinate + Side * (RoadWidth * 0.5f + 650.0f), Along, 0.0f);
                AddBox(StreetFurniture, Position + FVector(0.0f, 0.0f, 250.0f), FVector(35.0f, 35.0f, 500.0f));
                AddBox(StreetFurniture, Position + FVector(0.0f, 0.0f, 510.0f), FVector(180.0f, 55.0f, 35.0f));
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("OWCityMap: generated district with %d building instances."), Buildings->GetInstanceCount());
}