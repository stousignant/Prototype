// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "Kismet/GameplayStatics.h"

APrototypeGameMode::APrototypeGameMode(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MyCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Use our custom HUD class
	HUDClass = APrototypeHUD::StaticClass();

    // Set the default values 
    EnergyCount = 0.0f;
}

void APrototypeGameMode::Tick(float DeltaSeconds)
{
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // If the character still has power
    /*if (MyCharacter-> == 0.f)
    {
        // Decrease the character's power
        //MyCharacter->PowerLevel = FMath::FInterpTo(MyCharacter->PowerLevel, 0.f, DeltaSeconds, DecayRate);
    }
    else
    {
        SetCurrentState(EPrototypePlayState::EGameOver);
    }*/
}

void APrototypeGameMode::SetCurrentState(EPrototypePlayState NewState)
{
    CurrentState = NewState;

    HandleNewState(NewState);
}

void APrototypeGameMode::HandleNewState(EPrototypePlayState NewState)
{
    switch (NewState)
    {
    // When we're playing, the spawn volumes can spawn
    case EPrototypePlayState::EPlaying:
        for (ASpawnVolume* Volume : SpawnVolumeActors)
        {
            Volume->EnableSpawning();
        }
        break;
    // If the game is over, the spawn volumes should deactivate
    case EPrototypePlayState::EGameOver:
    {
        for (ASpawnVolume* Volume : SpawnVolumeActors)
        {
            Volume->DisableSpawning();
        }
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        PlayerController->SetCinematicMode(true, true, true);
    }
    break;
    // 
    case EPrototypePlayState::EUnknown:
    default:
        // do nothing
        break;
    }
}

void APrototypeGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Find all spawn volume actors
    TArray<AActor*> FoundSpawnVolumeActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);

    for (auto Actor : FoundSpawnVolumeActors)
    {
        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
        if (SpawnVolumeActor)
        {
            SpawnVolumeActors.Add(SpawnVolumeActor);
        }
    }

    // Find all death floor actors
    TArray<AActor*> FoundDeathFloorActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathFloor::StaticClass(), FoundDeathFloorActors);

    for (auto Actor : FoundDeathFloorActors)
    {
        ADeathFloor* DeathFloorActor = Cast<ADeathFloor>(Actor);
        if (DeathFloorActor)
        {
            DeathFloorActors.Add(DeathFloorActor);
        }
    }

    SetCurrentState(EPrototypePlayState::EPlaying);
}