#include "Prototype.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"
#include "DynamicEnvAsset.h"
#include "Kismet/GameplayStatics.h"

APrototypeGameMode::APrototypeGameMode(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
    
	// Use our custom HUD class
	HUDClass = APrototypeHUD::StaticClass();

    // Set the default values 
    EnergyMax = 2.0f;
    EnergyCount = 0.0f;
    ExplosionMax = 3.0f;
    ExplosionCount = 0.0f;
    
}

void APrototypeGameMode::Tick(float DeltaSeconds)
{
    /*
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // If the number of explosions has not meet the maximum
    if (ExplosionCount >= ExplosionMax)
    {
        SetCurrentState(EPrototypePlayState::EGameOver);
    }
    else
    {
        
    }*/
}

float APrototypeGameMode::GetEnergyCount()
{
    return EnergyCount;
}

void APrototypeGameMode::IncrementEnergy(int value)
{
    EnergyCount += value;

    if (EnergyCount >= EnergyMax)
    {
        SetCurrentState(EPrototypePlayState::EGameWon);
    }
}

float APrototypeGameMode::GetExplosionCount()
{
    return ExplosionCount;
}

void APrototypeGameMode::IncrementExplosion(int value)
{
    ExplosionCount += value;

    // TODO ENV
    // Find all the dynamic environment assets
    TArray<AActor*> FoundDynamicEnvAssetActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADynamicEnvAsset::StaticClass(), FoundDynamicEnvAssetActors);

    for (auto Actor : FoundDynamicEnvAssetActors)
    {
        ADynamicEnvAsset* DynamicEnvAsset = Cast<ADynamicEnvAsset>(Actor);
        if (DynamicEnvAsset)
        {
            //DynamicEnvAsset
        }
    }

    if (ExplosionCount >= ExplosionMax)
    {
        SetCurrentState(EPrototypePlayState::EGameOver);
    }
}

void APrototypeGameMode::SetCurrentState(EPrototypePlayState NewState)
{
    // Only apply the changes if needed
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        HandleNewState(NewState);
    }
}

void APrototypeGameMode::HandleNewState(EPrototypePlayState NewState)
{
    switch (NewState)
    {
    // When launching the game
    case EPrototypePlayState::ETutorial:
    {
        // Do nothing
    }
    break;
    // When we're playing, the spawn volumes can spawn
    case EPrototypePlayState::EPlaying:
    {
        ToggleSpawnVolumes(true);
    }
    break;
    // If the game is over, the spawn volumes should deactivate
    case EPrototypePlayState::EGameOver:
    {
        ToggleSpawnVolumes(false);

        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        PlayerController->SetCinematicMode(true, true, false);
    }
    break;
    // If the game is over, the spawn volumes should deactivate
    case EPrototypePlayState::EGameWon:
    {
        // Do nothing, let player have fun
    }
    break;
    // 
    case EPrototypePlayState::EUnknown:
    {
        // Do nothing
    }
    break;
    default:
    {
        // Do nothing
    }
    break;
    }

    // Play music related to the play state
    // Find the music player actor
    TArray<AActor*> FoundMusicPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMusicPlayer::StaticClass(), FoundMusicPlayerActors);

    for (auto Actor : FoundMusicPlayerActors)
    {
        AMusicPlayer* MusicPlayerActor = Cast<AMusicPlayer>(Actor);
        if (MusicPlayerActor)
        {
            MusicPlayerActor->PlayMusic(NewState);
        }
    }
}

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();

    SetCurrentState(EPrototypePlayState::ETutorial);
}

void APrototypeGameMode::ToggleSpawnVolumes(bool Toggle)
{
    // Find all spawn volume actors
    TArray<AActor*> FoundSpawnVolumeActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);

    // Enable spawning for each spawn volume
    for (auto Actor : FoundSpawnVolumeActors)
    {
        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
        if (SpawnVolumeActor)
        {
            SpawnVolumeActor->ToggleSpawning(Toggle);            
        }
    }
}