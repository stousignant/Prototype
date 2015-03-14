#include "Prototype.h"
#include "EnergyPickup.h"
#include "PrototypeGameMode.h"
#include "Engine.h"
#include "SpawnVolume.h"
#include "PrototypeCharacter.h"

const float SPEED_LEVEL_MULTIPLIER = 6.0f;

AEnergyPickup::AEnergyPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Default speed
    SpeedLevel = 0.0f;
    
    // Create the static mesh component
    BeamMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("BeamMesh"));
    BeamMesh->SetSimulatePhysics(false);
    BeamMesh->AttachTo(RootComponent);

    // Create the static mesh component
    InvaderMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("InvaderMesh"));
    InvaderMesh->SetSimulatePhysics(false);
    InvaderMesh->AttachTo(RootComponent);

    // Create the particle system component
    SphereParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("SphereParticle"));
    SphereParticle->SetSimulatePhysics(false);
    SphereParticle->AttachTo(RootComponent);

    // Create the particle system component
    BeamParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("BeamParticle"));
    BeamParticle->SetSimulatePhysics(false);
    BeamParticle->AttachTo(RootComponent);

    //
    bHasBeenPickedUp = false;

    // Make the energy tickable
    PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void AEnergyPickup::Tick(float DeltaSeconds)
{    
    FallSpeed = (SpeedLevel * DeltaSeconds * 60);
    SetActorLocation(GetActorLocation() - FVector(0, 0, FallSpeed/2));
    SetActorRotation(GetActorRotation() + FRotator(0.0f, DeltaSeconds * 5, 0.0f));
}

void AEnergyPickup::Absorb()
{
    // Increment the energy counter
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy();

    // Show particle effect
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation(), true);

    // Play explosion sound
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    
    // Hide the actor
    PickupMesh->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
    BeamMesh->SetHiddenInGame(true);
    BeamMesh->DestroyComponent();
    InvaderMesh->SetHiddenInGame(true);
    SphereParticle->SetHiddenInGame(true);
    BeamParticle->SetHiddenInGame(true);
    bHasBeenPickedUp = true;

    // Increase speed
    SpeedLevel = SpeedLevel * SPEED_LEVEL_MULTIPLIER;
}
/*
void AEnergyPickup::OnPickedUp_Implementation()
{
    // Call the parent implementation of OnPickedUp first
    //Super::OnPickedUp_Implementation();

    // Increment the energy counter
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy();

    // Show particle effect
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), GetActorRotation(), true);

    // Play explosion sound
    UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

    // Destroy the energy
    //Destroy();

    // Hide the actor
    PickupMesh->SetRelativeScale3D(FVector(0.1f,0.1f,0.1f));
    BeamMesh->SetHiddenInGame(true);
    InvaderMesh->SetHiddenInGame(true);
    bHasBeenPickedUp = true;

    // Increase speed
    SpeedLevel = SpeedLevel * SPEED_LEVEL_MULTIPLIER;

    
    // If early game, adjust spawn delay
    //if (MyGameMode->GetCurrentState() == EPrototypePlayState::EEarlyGame)
    //{
        // Find the spawn volume
    //    TArray<AActor*> FoundSpawnVolumeActors;
    //    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);
    //
    //    // Enable spawning for each spawn volume
    //    for (auto Actor : FoundSpawnVolumeActors)
    //    {
    //        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
    //        if (SpawnVolumeActor)
    //        {
    //            // Find other energy pickups
    //            TArray<AActor*> FoundEnergyPickupActors;
    //            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyPickup::StaticClass(), FoundEnergyPickupActors);
    //
    //            // If it was the only energy pickup in the arena
    //            if (FoundEnergyPickupActors.Num() == 0)
    //            {
    //                // Adjust the spawn delay based on the player's skills
    //                SpawnVolumeActor->AdjustSpawnDelay();
    //            }                
    //        }
    //    }
    ////}    
}*/

void AEnergyPickup::Explode()
{
    // Destroy the energy
    Destroy();

    // Spawn the next one
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->SpawnEnergyWithDelay();

    if (!bHasBeenPickedUp)
    {
        // Increment the missed energy counter
        MyGameMode->IncrementExplosion();

        // Play explosion sound
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

        // If not game over
        if (MyGameMode->GetCurrentState() != EPrototypePlayState::EGameOver)
        {
            // Play warning sound
            UGameplayStatics::PlaySoundAtLocation(this, WarningSound, GetActorLocation());
        }        
    }
}


void AEnergyPickup::ReceiveHit(class UPrimitiveComponent* MyComp,
                                AActor* Other,
                                class UPrimitiveComponent* OtherComp,
                                bool bSelfMoved,
                                FVector HitLocation,
                                FVector HitNormal,
                                FVector NormalImpulse,
                                const FHitResult & Hit)
{
    // Parent call
    Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // * PLAYER *
    // Cast the collected actor to APrototypeCharacter
    APrototypeCharacter* const TestCharacter = Cast<APrototypeCharacter>(Other);

    // if the cast is successful, and the energy is valid and active
    if (TestCharacter && !TestCharacter->IsPendingKill() && MyComp->GetName() == "InvaderMesh")
    {
        TestCharacter->System0731();
    }
}



