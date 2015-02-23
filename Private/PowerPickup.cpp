#include "Prototype.h"
#include "PowerPickup.h"
#include "PrototypeCharacter.h"

APowerPickup::APowerPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Make the power pickup tickable
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void APowerPickup::Tick(float DeltaSeconds)
{
    SetActorRotation(GetActorRotation() + FRotator(0.0f, 1.0f, 0.0f));
}

void APowerPickup::OnPickedUp_Implementation()
{
    // Call the parent implementation of OnPickedUp first
    Super::OnPickedUp_Implementation();

    // Play pick up sound
    UGameplayStatics::PlaySoundAtLocation(this, PowerPickupSound, GetActorLocation());

    // Deactivate the power pickup
    bIsActive = false;

    // Destroy the power pickup
    Destroy();
}

void APowerPickup::ReceiveHit(
    class UPrimitiveComponent * MyComp,
    AActor * Other,
    class UPrimitiveComponent * OtherComp,
    bool bSelfMoved,
    FVector HitLocation,
    FVector HitNormal,
    FVector NormalImpulse,
    const FHitResult & Hit)
{
    // Parent call
    Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // If other actor is the player
    APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(Other);
    if (TestCharacter)
    {
        // Pick up
        this->OnPickedUp();

        // Increment character stats
        TestCharacter->StatsCount++;
    }
}


