#include "Prototype.h"
#include "DeathFloor.h"
#include "EnergyPickup.h"
#include "PrototypeCharacter.h"
#include "Engine.h"

ADeathFloor::ADeathFloor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the pickup in the level
    WhereToSpawn = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("WhereToSpawn"));

    // Set the StaticMeshComponent as the root component
    RootComponent = WhereToSpawn;

    // Make the DeathFloor tickable
    PrimaryActorTick.bCanEverTick = true;
}

void ADeathFloor::Tick(float DeltaSeconds)
{
    // Get all overlapping Actors and store them in a CollectedActors array
    TArray<AActor*> CollectedActors;
    WhereToSpawn->GetOverlappingActors(CollectedActors);
        
    // For each actor collected
    for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
    {
        // * ENERGY *
        // Cast the collected actor to AEnergyPickup
        AEnergyPickup* const TestEnergy = Cast<AEnergyPickup>(CollectedActors[iCollected]);

        // if the cast is successful, and the energy is valid and active
        if (TestEnergy && !TestEnergy->IsPendingKill() && TestEnergy->bIsActive)
        {
            TestEnergy->Destroy();
        }

        // * PLAYER *
        // Cast the collected actor to APrototypeCharacter
        APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(CollectedActors[iCollected]);

        // if the cast is successful, and the player is valid and active
        if (TestCharacter && !TestCharacter->IsPendingKill())
        {
            TestCharacter->Die();
        }
    }
}


