#pragma once

#include "GameFramework/GameMode.h"
#include "SpawnVolume.h"
#include "DeathFloor.h"
#include "PrototypeGameMode.generated.h"

enum class EPrototypePlayState : short
{
    EPlaying,
    EGameOver,
    EUnknown
};

UCLASS(minimalapi)
class APrototypeGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    // Constructor
    APrototypeGameMode(const FObjectInitializer& ObjectInitializer);

    //////////////////////////////////////////////////////////////////////////
    // Update
    /** Tick */
    virtual void Tick(float DeltaSeconds) override;

    /** Number of explosions before the game ends */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
    float ExplosionMax;

    /** Amount of energy scanned by the player */
    float EnergyCount;

    /** Amount of energy exploded by hitting the deathfloor */
    float ExplosionCount;

    EPrototypePlayState GetCurrentState() const;
    void SetCurrentState(EPrototypePlayState NewState);

    virtual void BeginPlay() override;

private:

    // List of spawn volume actors in the scene
    TArray<ASpawnVolume*> SpawnVolumeActors;

    // List of death floor actors in the scene
    TArray<ADeathFloor*> DeathFloorActors;

    EPrototypePlayState CurrentState;

    void HandleNewState(EPrototypePlayState NewState);

};

FORCEINLINE EPrototypePlayState APrototypeGameMode::GetCurrentState() const
{
    return CurrentState;
}





