#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

/**
*
*/
UCLASS()
class PROTOTYPE_API ASpawnVolume : public AActor
{
    GENERATED_BODY()

public:
    // Constructor
    ASpawnVolume(const FObjectInitializer& ObjectInitializer);

    /** BoxComponent to specify the spawning area within the level */
    UPROPERTY(VisibleInstanceOnly, Category = Spawning)
    UBoxComponent* WhereToSpawn;

    /** The pickup to spawn */
    UPROPERTY(EditAnywhere, Category = Spawning)
    TSubclassOf<class APickup> WhatToSpawn;

    /** The current spawn delay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDelay;

    /** Finds a random point within the BoxComponent */
    UFUNCTION(BlueprintPure, Category = Spawning)
    FVector GetRandomPointInVolume();

    /** Speed of the object spawned */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnedSpeedLevel;
    
    /** Minimal distance for object to be spawn between them */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDistance;

    /** Speed increment */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpeedIncrement;

    /** Spawn delay decrement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDelayDecrement;

    /** Spawn distance increment */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDistanceIncrement;

    /** Counter to keep track of the number of object spawned */
    float SpawnCounter;

    /** Last random location generated */
    FVector LastRandomLocation;

    /** Is first spawn */
    bool bIsFirstSpawn;

    virtual void Tick(float DeltaSeconds) override;

    void ToggleSpawning(bool Toggle);


private:

    /** Whether or not spawning is enabled */
    bool bSpawningEnabled;

    /** Handles the spawning of a new pickup */
    void SpawnPickup();

    /** The timer for when to spawn the pickup */
    float SpawnTime;

};
