#pragma once

#include "GameFramework/GameMode.h"
#include "SpawnVolume.h"
#include "DeathFloor.h"
#include "PrototypeGameMode.generated.h"

enum class EPrototypePlayState : short
{
    EUnknown,
    ETutorial,
    EEarlyGame,
    EGameOver,
    EGameWon,
    ELateGame
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

    /** Number of energy to end the game */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
    float EnergyMax;

    /** Number of explosions before the game ends */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
    float ExplosionMax;
    
    /** Get the current game mode state */
    EPrototypePlayState GetCurrentState() const;

    /** Set a new game mode state */
    void SetCurrentState(EPrototypePlayState NewState);

    /** Get the energy count */
    float GetEnergyCount();

    /** Increment the energy count (default + 1) */
    void IncrementEnergy(int value = 1);

    /** Get the explosion count */
    float GetExplosionCount();

    /** Increment the explosion count (default + 1) */
    void IncrementExplosion(int value = 1);

    /** Override function used when game is launched */
    virtual void BeginPlay() override;
    
private:

    /** Enable-Disable spawning for all spawn volumes */
    void ToggleSpawnVolumes(bool Toggle);
    
    /** Current game mode state */
    EPrototypePlayState CurrentState;

    /** Handle a new game mode state */
    void HandleNewState(EPrototypePlayState NewState);

    /** Amount of energy scanned by the player */
    float EnergyCount;

    /** Amount of energy exploded by hitting the deathfloor */
    float ExplosionCount;

};

FORCEINLINE EPrototypePlayState APrototypeGameMode::GetCurrentState() const
{
    return CurrentState;
}





