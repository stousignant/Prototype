#include "Prototype.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"
#include "DynamicEnvAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

const int NORMAL_MODE = 5;
const int HARD_MODE = 20;
const int VERYHARD_MODE = 50;
const int ULTIMATE_MODE = 100;

APrototypeGameMode::APrototypeGameMode(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
    
	// Use our custom HUD class
	HUDClass = APrototypeHUD::StaticClass();

    // Set the default values 
    EnergyMax = NORMAL_MODE;
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
    if (GetCurrentState() == EPrototypePlayState::ENormalMode || 
        GetCurrentState() == EPrototypePlayState::EHardMode || 
        GetCurrentState() == EPrototypePlayState::EVeryHardMode || 
        GetCurrentState() == EPrototypePlayState::EUltimateMode)
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

    // Increase difficulty
    SpawnVolumeActor->SpawnedSpeedLevel = SpawnVolumeActor->SpawnedSpeedLevel + (SpawnVolumeActor->SpawnSpeedIncrement * value);
    SpawnVolumeActor->SpawnDistance = SpawnVolumeActor->SpawnDistance + (SpawnVolumeActor->SpawnDistanceIncrement * value);

    // Check if game won
    if (EnergyCount >= EnergyMax)
    {
        SetCurrentState(EPrototypePlayState::EGameWon);
    }
}

float APrototypeGameMode::GetExplosionCount()
{
    return ExplosionCount;
}

// HACK
void APrototypeGameMode::SetExplosionCount(float value)
{
    ExplosionCount = value;
}

void APrototypeGameMode::IncrementExplosion(int value)
{
    ExplosionCount += value;

    // Increase difficulty
    SpawnVolumeActor->SpawnedSpeedLevel = SpawnVolumeActor->SpawnedSpeedLevel + (SpawnVolumeActor->SpawnSpeedIncrement * value);
    SpawnVolumeActor->SpawnDistance = SpawnVolumeActor->SpawnDistance + (SpawnVolumeActor->SpawnDistanceIncrement * value);
    
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

void APrototypeGameMode::SetNextState()
{
    if (EnergyMax == NORMAL_MODE)
    {
        SetCurrentState(EPrototypePlayState::EHardMode);
    }
    else if (EnergyMax == HARD_MODE)
    {
        SetCurrentState(EPrototypePlayState::EVeryHardMode);
    }
    else if (EnergyMax == VERYHARD_MODE)
    {
        SetCurrentState(EPrototypePlayState::EUltimateMode);
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
    case EPrototypePlayState::ENormalMode:
    {
        // First pick up spawn
        SpawnEnergy();

        // Play early game music
        MusicPlayerActor->PlayNormalModeMusic();

        // Current mode string
        CurrentModeString = FString(TEXT("NORMAL MODE"));
        NextModeString = FString(TEXT("HARD MODE"));

        // Energy max
        EnergyMax = NORMAL_MODE;
    }
    break;
    //
    case EPrototypePlayState::EHardMode:
    {
        // Get the character and unblock mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = false;

        // Play late game music
        MusicPlayerActor->PlayHardModeMusic();

        // Current mode string
        CurrentModeString = FString(TEXT("HARD MODE"));
        NextModeString = FString(TEXT("VERY HARD MODE"));

        // Energy max
        EnergyMax = HARD_MODE;
    }
    break;
    //
    case EPrototypePlayState::EVeryHardMode:
    {
        // Get the character and unblock mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = false;

        // Play late game music
        MusicPlayerActor->PlayVeryHardModeMusic();

        // Current mode string
        CurrentModeString = FString(TEXT("VERY HARD MODE"));
        NextModeString = FString(TEXT("ULTIMATE MODE"));

        // Energy max
        EnergyMax = VERYHARD_MODE;

        // Increase difficulty
        SpawnVolumeActor->SpawnSpeedIncrement = 1.0f;
    }
    break;
    //
    case EPrototypePlayState::EUltimateMode:
    {
        // Get the character and unblock mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = false;

        // Play late game music
        MusicPlayerActor->PlayUltimateModeMusic();

        // Current mode string
        CurrentModeString = FString(TEXT("ULTIMATE MODE"));

        // Energy max
        EnergyMax = ULTIMATE_MODE;

        // Increase difficulty
        SpawnVolumeActor->bIsUltimateMode = true;
    }
    break;
    //
    case EPrototypePlayState::EGameOver:
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        PlayerController->SetCinematicMode(true, true, false);

        // Get the character and end overload if needed
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        if (MyCharacter->bIsOverloaded)
        {
            MyCharacter->EndOverload();
        }        

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

        // End overload if needed
        if (MyCharacter->bIsOverloaded)
        {
            MyCharacter->EndOverload();
        }

        // Play game won music
        MusicPlayerActor->PlayGameWonMusic();
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
    GetWorldTimerManager().SetTimer(UniqueHandle, TimerDelegate, 1.f, false);    
}

void APrototypeGameMode::SpawnEnergy()
{
    SpawnVolumeActor->SpawnPickup();
}