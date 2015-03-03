#include "Prototype.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"

AEnergyPickup::AEnergyPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Default speed of the fall
    SpeedLevel = 5.0f;

    // Make the energy tickable
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void AEnergyPickup::Tick(float DeltaSeconds)
{
    SetActorLocation(GetActorLocation() - FVector(0, 0, SpeedLevel));
}

void AEnergyPickup::OnPickedUp_Implementation()
{
    // Call the parent implementation of OnPickedUp first
    Super::OnPickedUp_Implementation();

    // Increment the energy counter
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->EnergyCount++;   

    // Show particle effect
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation(), true);

    // Play explosion sound
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

    // Destroy the energy
    Destroy();
}

void AEnergyPickup::Explode()
{
    // Destroy the energy
    Destroy();

    // Increment the missed energy counter
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->ExplosionCount++;    

    // Play explosion sound
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
}



