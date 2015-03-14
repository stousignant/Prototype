#include "Prototype.h"
#include "SpawnVolume.h"
#include "Pickup.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"
#include "Engine.h"

// Difficulty caps
const float SPAWN_DISTANCE_MAX = 25000.0f;
const float SPAWN_SPEED_MAX = 40.0f;
const float SPAWN_DELAY_MAX = 13.0f;

ASpawnVolume::ASpawnVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the pickup in the level
    WhereToSpawn = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("WhereToSpawn"));

    // Set the StaticMeshComponent as the root component
    RootComponent = WhereToSpawn;

    // Set the spawn delay range and get the first SpawnDelay
    SpawnDelay = 50.0f;
    SpawnDelayMultiplier = 1.0f;
    
    // Set the default speed level for the spawning objects
    SpawnedSpeedLevel = 10.0f;

    // Set the default speed increment
    SpeedIncrement = 1.0f;

    // Set the default spawn delay decrement
    SpawnDelayDecrement = 1.0f;

    // Set the default spawn distance values
    SpawnDistance = 14000.0f;
    SpawnDistanceIncrement = 500.0f;

    // Set the default
    bIsFirstSpawn = true;

    // Make the SpawnVolume tickable
    PrimaryActorTick.bCanEverTick = true;
}

void ASpawnVolume::SpawnPickup()
{
    // If we have set something to spawn:
    if (WhatToSpawn != NULL)
    {
        // Check for a valid World: 
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
                SpawnedEnergy->SpeedLevel = SpawnedSpeedLevel;
            }

            // Increase difficulty but not over threshold
            SpawnedSpeedLevel = SpawnedSpeedLevel >= SPAWN_SPEED_MAX ? SPAWN_SPEED_MAX : SpawnedSpeedLevel + SpeedIncrement;

            // Increment the spawn counter
            SpawnCounter++;

            // Stop spawning if maximum is reached in the early game
            APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            if (MyGameMode->GetCurrentState() == EPrototypePlayState::EEarlyGame && SpawnCounter >= MyGameMode->EnergyMax)
            {
                ToggleSpawning(false);
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
        if (MyGameMode->GetCurrentState() == EPrototypePlayState::EEarlyGame)
        {
            do
            {
                // The random spawn location will fall between the min and max X, Y, and Z
                RandomLocation.X = FMath::FRandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
                RandomLocation.Y = FMath::FRandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);
                RandomLocation.Z = FMath::FRandRange(Origin.Z, Origin.Z);

            } while (FVector::Dist(LastRandomLocation, RandomLocation) > (SpawnDistance * 0.75f) || FVector::Dist(LastRandomLocation, RandomLocation) < (SpawnDistance * 0.5f));
        }
        else
        {
            do
            {
                // The random spawn location will fall between the min and max X, Y, and Z
                RandomLocation.X = FMath::FRandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X);
                RandomLocation.Y = FMath::FRandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y);
                RandomLocation.Z = FMath::FRandRange(Origin.Z, Origin.Z);

            } while (FVector::Dist(LastRandomLocation, RandomLocation) < SpawnDistance);
        }        
    }    
        
    // Return the random spawn location
    LastRandomLocation = RandomLocation;
    return RandomLocation;
}

void ASpawnVolume::Tick(float DeltaSeconds)
{
    // If spawning is not enabled, do nothing
    if (!bSpawningEnabled)
        return;

    // Always add delta time to our Spawn Time
    SpawnTime += DeltaSeconds * SpawnDelayMultiplier;

    bool bShouldSpawn = (SpawnTime > SpawnDelay);

    if (bShouldSpawn)
    {
        SpawnPickup();

        // Reset spawn timer
        SpawnTime = 0.0f;
        SpawnDelayMultiplier = 1.0f;

        // Reduce spawn delay per spawn
        SpawnDelay = SpawnDelay <= SPAWN_DELAY_MAX ? SPAWN_DELAY_MAX : SpawnDelay - SpawnDelayDecrement;

        // Increment the spawn distance per spawn but not over threshold
        SpawnDistance = SpawnDistance >= SPAWN_DISTANCE_MAX ? SPAWN_DISTANCE_MAX : SpawnDistance + SpawnDistanceIncrement;
    }
}

void ASpawnVolume::ToggleSpawning(bool Toggle)
{
    bSpawningEnabled = Toggle;

    // If early game, spawn now
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (MyGameMode->GetCurrentState() == EPrototypePlayState::EEarlyGame)
    {
        // Start spawning immediately
        SpawnTime = SpawnDelay;
    }
    else if (MyGameMode->GetCurrentState() == EPrototypePlayState::ELateGame)
    {
        // Start spawning soon
        SpawnTime = SpawnDelay / 2;
    }    
}

void ASpawnVolume::AdjustSpawnDelay()
{
    // best = 20x, worst = 1x
    SpawnDelayMultiplier = SpawnDelay / SpawnTime;
}






