#include "Prototype.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"
#include "Engine.h"

AEnergyPickup::AEnergyPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Default speed
    SpeedLevel = 0.0f;

    // Create the static mesh component
    BeamMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BeamMesh"));
    BeamMesh->SetSimulatePhysics(false);
    BeamMesh->AttachTo(RootComponent);

    // Make the energy tickable
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void AEnergyPickup::Tick(float DeltaSeconds)
{    
    FallSpeed = (SpeedLevel * DeltaSeconds * 60);
    SetActorLocation(GetActorLocation() - FVector(0, 0, FallSpeed/2));
}

void AEnergyPickup::OnPickedUp_Implementation()
{
    // Call the parent implementation of OnPickedUp first
    Super::OnPickedUp_Implementation();

    // Increment the energy counter
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy();

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
    MyGameMode->IncrementExplosion();

    // Play explosion sound
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
}



