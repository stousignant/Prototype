#include "Prototype.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"
#include "DynamicEnvAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"

const float BEST_TUTORIAL_TIME_ATTACK = 9.49f;

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

    GameTime = 0.0f;
    TutorialCurrentTimeAttack = 0.0f;
    TutorialBestTimeAttack = BEST_TUTORIAL_TIME_ATTACK;
    TutorialTimeAttackTimer = 0.0;
    bShowTutorialTimeAttack = false;

    ExplosionWarningTimer = 0.0;
    bShowExplosionWarning = false;

    bIsSpawnVolumeSet = false;
}

void APrototypeGameMode::Tick(float DeltaSeconds)
{
    // Accumulate time for tutorial time attack
    if (GetCurrentState() == EPrototypePlayState::ETutorial)
    {
        TutorialCurrentTimeAttack += DeltaSeconds;
    }

    // Accumulate time for game time
    if (GetCurrentState() == EPrototypePlayState::EEarlyGame || GetCurrentState() == EPrototypePlayState::ELateGame)
    {
        GameTime += DeltaSeconds;
    }

    // Show tutorial time attack results
    if (TutorialTimeAttackTimer > 0.0f)
    {
        TutorialTimeAttackTimer -= DeltaSeconds;
        bShowTutorialTimeAttack = true;

        if (TutorialTimeAttackTimer <= 0.0f)
        {
            bShowTutorialTimeAttack = false;
        }
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

    // Find spawn volume
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
        // Show tutorial time attack for 4 seconds
        TutorialTimeAttackTimer = 4.0f;

        // First pick up spawn
        SpawnEnergy();
    }
    break;
    //
    case EPrototypePlayState::EGameOver:
    {
        // If the game is over, the spawn volumes should deactivate
        //ToggleSpawnVolumes(false);

        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        PlayerController->SetCinematicMode(true, true, false);
    }
    break;
    //
    case EPrototypePlayState::EGameWon:
    {
        // Get the character and block mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = true;
        
        // If the game is won, the spawn volumes should deactivate
        //ToggleSpawnVolumes(false);
    }
    break;
    //
    case EPrototypePlayState::ELateGame:
    {
        // Get the character and unblock mouvement
        APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        MyCharacter->bMovementBlocked = false;
        
        // If the game is won, the spawn volumes should deactivate
        //ToggleSpawnVolumes(true);
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

/*void APrototypeGameMode::ToggleSpawnVolumes(bool Toggle)
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
}*/

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