#include "Prototype.h"
#include "DeathFloor.h"
#include "EnergyPickup.h"
#include "PrototypeCharacter.h"
#include "Engine.h"

ADeathFloor::ADeathFloor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the death floor
    DeathFloorCollider = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("DeathFloorCollider"));

    // Set the StaticMeshComponent as the root component
    RootComponent = DeathFloorCollider;

    // Make the actor tickable
    PrimaryActorTick.bCanEverTick = true;

    // Default
    TimerDelay = 0.0f;
}

void ADeathFloor::Tick(float DeltaSeconds)
{
    // Count time
    TimerDelay += DeltaSeconds;

    // Execute tick each 0.5 second
    if (TimerDelay > 0.1f)
    {
        // Reset timer
        TimerDelay = 0.0f;

        // Get all overlapping Actors and store them in a CollectedActors array
        TArray<AActor*> CollectedActors;
        DeathFloorCollider->GetOverlappingActors(CollectedActors);

        // For each actor collected
        for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
        {
            // * ENERGY *
            // Cast the collected actor to AEnergyPickup
            AEnergyPickup* const TestEnergy = Cast<AEnergyPickup>(CollectedActors[iCollected]);

            // if the cast is successful, and the energy is valid and active
            if (TestEnergy && !TestEnergy->IsPendingKill() && TestEnergy->bIsActive)
            {
                TestEnergy->Explode();
            }

            // * PLAYER *
            // Cast the collected actor to APrototypeCharacter
            APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(CollectedActors[iCollected]);
            
            //bIsScanning = (testHitResult.GetActor() && testHitResult.GetActor()->GetClass()->IsChildOf(AEnergyPickup::StaticClass()));
            //CollectedActors[iCollected]->GetClass()->IsChildOf()

            // if the cast is successful, and the player is valid and active
            if (TestCharacter && !TestCharacter->IsPendingKill() && !TestCharacter->bIsDead)
            {
                TestCharacter->Die();
            }
        }
    }   
}

void ADeathFloor::ReceiveHit(
    class UPrimitiveComponent* MyComp,
    AActor* Other,
    class UPrimitiveComponent* OtherComp,
    bool bSelfMoved,
    FVector HitLocation,
    FVector HitNormal,
    FVector NormalImpulse,
    const FHitResult & Hit)
{
    // WARNING :: Not currently used since DeathFloor isn't blocking..

    // Parent call
    Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // * ENERGY *
    // Cast the collected actor to AEnergyPickup
    AEnergyPickup* const TestEnergy = Cast<AEnergyPickup>(Other);

    // if the cast is successful, and the energy is valid and active
    if (TestEnergy && !TestEnergy->IsPendingKill() && TestEnergy->bIsActive)
    {
        TestEnergy->Explode();
    }

    // * PLAYER *
    // If other actor is the player
    APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(Other);

    // if the cast is successful, and the player is valid and active
    if (TestCharacter && !TestCharacter->IsPendingKill() && !TestCharacter->bIsDead)
    {
        TestCharacter->Die();
    }
}


