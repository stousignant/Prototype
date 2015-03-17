#include "Prototype.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"
#include "DynamicEnvAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

APrototypeGameMode::APrototypeGameMode(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
    
	// Use our custom HUD class
	HUDClass = APrototypeHUD::StaticClass();

    // Set the default values 
    EnergyMax = 5.0f;
    EnergyCount = 0.0f;
    ExplosionMax = 3.0f;
    ExplosionCount = 0.0f;  

    // Game timer
    GameTime = 0.0f;

    // Explosion
    ExplosionWarningTimer = 0.0;
    bShowExplosionWarning = false;

    // Basic actors
    bIsSpawnVolumeSet = false;
    bIsMusicPlayerSet = false;

    // Overload
    bOverloadMusicStarted = false;
}

void APrototypeGameMode::Tick(float DeltaSeconds)
{
    // Update for overload check
    UpdateOverload();
    
    // Accumulate time for game time
    if (GetCurrentState() == EPrototypePlayState::EEarlyGame || GetCurrentState() == EPrototypePlayState::ELateGame)
    {
        GameTime += DeltaSeconds;
    }

    // Show explosion warning
    if (ExplosionWarningTimer > 0.0f)
    {
        ExplosionWarningTimer -= DeltaSeconds;
        bShowExplosionWarning = true;

        if (ExplosionWarningTimer <= 0.0f)
        {
            bShowExplosionWarning = false;
        }
    }
    
    // Set basic actors
    SetSpawnVolumeActor();
    SetMusicPlayerActor();
}

void APrototypeGameMode::UpdateOverload()
{
    // Overload
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    if (!bOverloadMusicStarted)
    {
        if (MyCharacter->bOverloadStarted)
        {
            MusicPlayerActor->PlayOverloadMusic();
            bOverloadMusicStarted = true;
        }
    }

    if (MyCharacter->bOverloadEnded)
    {
        MusicPlayerActor->StopOverloadMusic();
        bOverloadMusicStarted = false;

        MyCharacter->bOverloadEnded = false;
    }
}

void APrototypeGameMode::SetSpawnVolumeActor()
{
    // Find spawn volume actor
    if (!bIsSpawnVolumeSet)
    {
        TArray<AActor*> FoundSpawnVolumeActors;

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);

        // Enable spawning for each spawn volume
        for (auto Actor : FoundSpawnVolumeActors)
        {
            SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
            bIsSpawnVolumeSet = true;
        }
    }
}

void APrototypeGameMode::SetMusicPlayerActor()
{
    // Find the music player actor
    if (!bIsMusicPlayerSet)
    {
        TArray<AActor*> FoundMusicPlayerActors;

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMusicPlayer::StaticClass(), FoundMusicPlayerActors);

        for (auto Actor : FoundMusicPlayerActors)
        {
            MusicPlayerActor = Cast<AMusicPlayer>(Actor);
            MusicPlayerActor->PlayTutorialMusic();
            bIsMusicPlayerSet = true;
        }
    }
}

float APrototypeGameMode::GetEnergyCount()
{
    return EnergyCount;
}

void APrototypeGameMode::IncrementEnergy(int value)
{
    EnergyCount += value;

    // Check if game won
    if (GetCurrentState() == EPrototypePlayState::EEarlyGame && EnergyCount >= EnergyMax)
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
    
    // Check if game over
    if (ExplosionCount >= ExplosionMax)
    {
        SetCurrentState(EPrototypePlayState::EGameOver);
    }
    // Signal warning
    else
    {
        // Show warning for planet life loss 3 seconds
        ExplosionWarningTimer = 3.0f;
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
    case EPrototypePlayState::EEarlyGame:
    {
        // First pick up spawn
        SpawnEnergy();

        // Play early game music
        MusicPlayerActor->PlayEarlyGameMusic();
    }
    break;
    //
    case EPrototypePlayState::EGameOver:
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        PlayerController->SetCinematicMode(true, true, false);

        // Play game over music
        MusicPlayerActor->PlayGameOverMusic();
    }
    break;
    //
    case EPrototypePlayState::EGameWon:
    {
        // Get the character and block mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = true;

        // Play game won music
        MusicPlayerActor->PlayGameWonMusic();
    }
    break;
    //
    case EPrototypePlayState::ELateGame:
    {
        // Get the character and unblock mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = false;

        // Play late game music
        MusicPlayerActor->PlayLateGameMusic();
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
}

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();

    SetCurrentState(EPrototypePlayState::ETutorial);
}

void APrototypeGameMode::SpawnEnergyWithDelay()
{
    // Spawn pickup after a 3 second delay
    FTimerHandle UniqueHandle;
    FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &APrototypeGameMode::SpawnEnergy);
    GetWorldTimerManager().SetTimer(UniqueHandle, TimerDelegate, 3.f, false);    
}

void APrototypeGameMode::SpawnEnergy()
{
    SpawnVolumeActor->SpawnPickup();
}