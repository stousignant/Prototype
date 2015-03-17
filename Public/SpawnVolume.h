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

    /** */
    virtual void Tick(float DeltaSeconds) override;

    /** BoxComponent to specify the spawning area within the level */
    UPROPERTY(VisibleInstanceOnly, Category = Spawning)
    UBoxComponent* WhereToSpawn;

    /** The pickup to spawn */
    UPROPERTY(EditAnywhere, Category = Spawning)
    TSubclassOf<class APickup> WhatToSpawn;
    
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
    float SpawnSpeedIncrement;

    /** Spawn distance increment */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float SpawnDistanceIncrement;

    /** */
    bool bIsUltimateMode;

    /** */
    bool bIsCharacterOverloaded;

    /** Last random location generated */
    FVector LastRandomLocation;

    /** Is first spawn */
    bool bIsFirstSpawn;

    /** Handles the spawning of a new pickup */
    void SpawnPickup();    


private:
    
    /** Generate random position */
    void GenerateRandomLocation();

};
