#include "Prototype.h"
#include "SpawnVolume.h"
#include "Pickup.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"
#include "Engine.h"

// Difficulty caps
const float SPAWN_DISTANCE_MAX = 25000.0f;
const float SPAWN_SPEED_MAX = 50.0f; // 40.0f = 14 sec ou 80.0f = 14 sec en overload
// 50.0f = 11.2 sec ou 100.0f = 11.2 sec en overload

ASpawnVolume::ASpawnVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the pickup in the level
    WhereToSpawn = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("WhereToSpawn"));

    // Set the StaticMeshComponent as the root component
    RootComponent = WhereToSpawn;
        
    // Set the default speed level for the spawning objects
    SpawnedSpeedLevel = 5.0f;

    // Set the default speed increment
    SpawnSpeedIncrement = 0.5f;
    
    // Set the default spawn distance values
    SpawnDistance = 14000.0f;
    SpawnDistanceIncrement = 500.0f;

    // Set the default
    bIsFirstSpawn = true;
    bIsUltimateMode = false;
    bIsCharacterOverloaded = false;

    // Make the SpawnVolume tickable
    PrimaryActorTick.bCanEverTick = false;
}

void ASpawnVolume::SpawnPickup()
{
    // If we have set something to spawn
    if (WhatToSpawn != NULL)
    {
        // Check for a valid World
        UWorld* const World = GetWorld();
        if (World)
        {
            // Set the spawn parameters
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;

            // Get a random location to spawn at
            FVector SpawnLocation = GetRandomPointInVolume();

            // Get a rotation for the spawned item
            FRotator SpawnRotation;
            SpawnRotation.Yaw = 0.0f;
            SpawnRotation.Pitch = 0.0f;
            SpawnRotation.Roll = 0.0f;

            // Spawn the pickup
            APickup* const SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

            AEnergyPickup* const SpawnedEnergy = Cast<AEnergyPickup>(SpawnedPickup);
            if (SpawnedEnergy)
            {
                // Check if speed is not over threshold
                if (SpawnedSpeedLevel >= SPAWN_SPEED_MAX)
                {
                    SpawnedSpeedLevel = SPAWN_SPEED_MAX;
                }

                // Double speed if in ultimate mode
                if (bIsUltimateMode)
                {
                    SpawnedSpeedLevel *= 2.0f;
                }

                if (bIsCharacterOverloaded)
                {
                    SpawnedSpeedLevel /= 2.0f;
                }

                // Attribute new speed
                SpawnedEnergy->SpeedLevel = SpawnedSpeedLevel;

                // Check if distance is not over threshold
                if (SpawnDistance >= SPAWN_DISTANCE_MAX)
                {
                    SpawnDistance = SPAWN_DISTANCE_MAX;
                }
            }                        
        }
    }
}

FVector ASpawnVolume::GetRandomPointInVolume()
{
    FVector RandomLocation;
    FVector Origin;
    FVector BoxExtent;

    // Get the SpawnVolume's origin and box extent
    Origin = WhereToSpawn->Bounds.Origin;
    BoxExtent = WhereToSpawn->Bounds.BoxExtent;
    
    // Is the first spawn
    if (bIsFirstSpawn)
    {
        // Spawn an easy first one
        RandomLocation.X = Origin.X + 1500.0f;
        RandomLocation.Y = Origin.Y - 17000.0f;
        RandomLocation.Z = Origin.Z;

        // Set the first spawn to false
        bIsFirstSpawn = false;
    }
    // All other spawn, be random
    else
    {
        // If early game, spawn close
        APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        float SpawnDistanceDifficulty = (MyGameMode->GetCurrentState() == EPrototypePlayState::ENormalMode) ? 1.0f : 1.5f;
        
        do
        {
            // The random spawn location will fall between the min and max X, Y, and Z
            RandomLocation.X = FMath::FRandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
            RandomLocation.Y = FMath::FRandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);
            RandomLocation.Z = FMath::FRandRange(Origin.Z, Origin.Z);

        } while (FVector::Dist(LastRandomLocation, RandomLocation) > (SpawnDistance * 0.75f * SpawnDistanceDifficulty) || FVector::Dist(LastRandomLocation, RandomLocation) < (SpawnDistance * 0.5f * SpawnDistanceDifficulty));
    }    
        
    // Return the random spawn location
    LastRandomLocation = RandomLocation;
    return RandomLocation;
}

void ASpawnVolume::Tick(float DeltaSeconds)
{
}






