#pragma once

#include "GameFramework/GameMode.h"
#include "SpawnVolume.h"
#include "DeathFloor.h"
#include "MusicPlayer.h"
#include "PrototypeGameMode.generated.h"

enum class EPrototypePlayState : short
{
    EUnknown,
    ETutorial,
    ENormalMode,
    EHardMode,
    EVeryHardMode,
    EUltimateMode,
    EGameOver,
    EGameWon,
    EGameClear
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

    /** */
    void UpdateOverload();

    /** */
    ASpawnVolume* SpawnVolumeActor;
    
    /** */
    bool bIsSpawnVolumeSet;

    /** */
    void SetSpawnVolumeActor();

    /** */
    AMusicPlayer* MusicPlayerActor;

    /** */
    bool bIsMusicPlayerSet;

    /** */
    void SetMusicPlayerActor();

    /** */
    FString CurrentModeString;

    /** */
    FString NextModeString;

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

    /** Set the next game mode state */
    void SetNextState();

    /** Get the energy count */
    float GetEnergyCount();

    /** Increment the energy count (default + 1) */
    void IncrementEnergy(int value = 1);

    /** Get the explosion count */
    float GetExplosionCount();

    /** Set the explosion count */
    void SetExplosionCount(float value);

    /** Increment the explosion count (default + 1) */
    void IncrementExplosion(int value = 1);

    /** Override function used when game is launched */
    virtual void BeginPlay() override;

    /** Time elapsed during the game */
    float GameTime;

    /** Show the planet life loss warning for secs */
    float ExplosionWarningTimer;

    /** Should show planet life loss warning */
    bool bShowExplosionWarning;

    /** Spawn an energy with a delay */
    void SpawnEnergyWithDelay();

    /** */
    bool bOverloadMusicStarted;



    
    
private:
         
    /** Spawn an energy  */
    void SpawnEnergy();

    /** Current game mode state */
    EPrototypePlayState CurrentState;

    /** Handle a new game mode state */
    void HandleNewState(EPrototypePlayState NewState);

    /** Amount of energy scanned by the player */
    float EnergyCount;

    /** Amount of energy exploded by hitting the deathfloor */
    float ExplosionCount;

    /** Increase stats of spawn */
    void IncreaseSpawnStats(float value);

};

FORCEINLINE EPrototypePlayState APrototypeGameMode::GetCurrentState() const
{
    return CurrentState;
}





