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

    /** Minimum spawn delay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDelayRangeLow;

    /** Maximum spawn delay */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDelayRangeHigh;

    /** Finds a random point within the BoxComponent */
    UFUNCTION(BlueprintPure, Category = Spawning)
    FVector GetRandomPointInVolume();

    /** Speed of the object spawned */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnedSpeedLevel;
    
    /** Counter to keep track of the number of object spawned */
    float SpawnCounter;

    virtual void Tick(float DeltaSeconds) override;

    void ToggleSpawning(bool Toggle);


private:

    /** Whether or not spawning is enabled */
    bool bSpawningEnabled;

    /** Calculates a random spawn delay */
    float GetRandomSpawnDelay();

    /** The current spawn delay */
    float SpawnDelay;

    /** Handles the spawning of a new pickup */
    void SpawnPickup();

    /** The timer for when to spawn the pickup */
    float SpawnTime;

};
