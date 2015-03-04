#pragma once

#include "GameFramework/GameMode.h"
#include "SpawnVolume.h"
#include "DeathFloor.h"
#include "PrototypeGameMode.generated.h"

enum class EPrototypePlayState : short
{
    EUnknown,
    ETutorial,
    EPlaying,
    EGameOver    
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

    /** Get the current game mode state */
    EPrototypePlayState GetCurrentState() const;

    /** Set a new game mode state */
    void SetCurrentState(EPrototypePlayState NewState);

    /** Override function used when game is launched */
    virtual void BeginPlay() override;
    
private:

    /** Enable-Disable spawning for all spawn volumes */
    void ToggleSpawnVolumes(bool Toggle);
    
    /** Current game mode state */
    EPrototypePlayState CurrentState;

    /** Handle a new game mode state */
    void HandleNewState(EPrototypePlayState NewState);

};

FORCEINLINE EPrototypePlayState APrototypeGameMode::GetCurrentState() const
{
    return CurrentState;
}





