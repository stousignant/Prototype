#include "Prototype.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"
#include "Engine.h"

AEnergyPickup::AEnergyPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Default values
    SpeedLevel = 0.0f;
    DecayRate = 0.0f;
    TimerDecay = 0.0f;

    // Make the energy tickable
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void AEnergyPickup::Tick(float DeltaSeconds)
{    
    FallSpeed = (SpeedLevel * DeltaSeconds * 50)/* - DecayRate*/;
    SetActorLocation(GetActorLocation() - FVector(0, 0, FallSpeed/2));
    
    TimerDecay += DeltaSeconds;

    // Is still in his first 20 seconds of spawning
    if (TimerDecay < 20.0f)
    {
        DecayRate += (0.03f * DeltaSeconds * 50) / TimerDecay;
    }
    

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("FallSpeed (%f) DeltaSeconds (%f)"), FallSpeed, DeltaSeconds));
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



