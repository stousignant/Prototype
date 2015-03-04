#include "Prototype.h"
#include "StartWall.h"
#include "PrototypeCharacter.h"
#include "PrototypeGameMode.h"
#include "Engine.h"

AStartWall::AStartWall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the start wall
    StartWallCollider = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("StartWallCollider"));

    // Set the StaticMeshComponent as the root component
    RootComponent = StartWallCollider;

    // Make the actor tickable
    PrimaryActorTick.bCanEverTick = true;
}

void AStartWall::Tick(float DeltaSeconds)
{
    // Get all overlapping Actors and store them in a CollectedActors array
    TArray<AActor*> CollectedActors;
    StartWallCollider->GetOverlappingActors(CollectedActors);

    // For each actor collected
    for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
    {
        // * PLAYER *
        // Cast the collected actor to APrototypeCharacter
        APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(CollectedActors[iCollected]);

        // if the cast is successful, and the player is valid and active
        if (TestCharacter && !TestCharacter->IsPendingKill())
        {
            // Start game mode
            APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            MyGameMode->SetCurrentState(EPrototypePlayState::EPlaying);

            // Stop ticking
            PrimaryActorTick.bCanEverTick = false;
        }
    }
}


