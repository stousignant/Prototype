#pragma once

#include "Pickup.h"
#include "EnergyPickup.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AEnergyPickup : public APickup
{
	GENERATED_BODY()
	
public:
    // Constructor
    AEnergyPickup(const FObjectInitializer& ObjectInitializer);
    
    // Static mesh component to represent the beam of the energy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    UStaticMeshComponent* BeamMesh;

    // Static mesh component to represent the invader of the energy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    UStaticMeshComponent* InvaderMesh;

    // Particle system component to represent the particle of the sphere
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    UParticleSystemComponent* SphereParticle;

    // Particle system component to represent the particle of the beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    UParticleSystemComponent* BeamParticle;

    //////////////////////////////////////////////////////////////////////////
    // Update
    /** Tick */
    virtual void Tick(float DeltaSeconds) override;

    // Speed parameter of the energy
    float SpeedLevel;

    // Falling decay rate
    float DecayRate;

    // Fall speed
    float FallSpeed;

    // Delay before stopping the decay
    float TimerDecay;

    // When player absorbed the energy
    void Absorb();
	
    // Happens when the energy hit the deathfloor
    void Explode();

    //
    bool bHasBeenPickedUp;

    /** Explosion particle effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
    UParticleSystem* ExplosionEffect;

    /** Sound to play when absorbed or destroyed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* ExplosionSound;

    /** Sound to play when destroyed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* WarningSound;

    /** Override the ReceiveHit function */
    virtual void ReceiveHit(class UPrimitiveComponent* MyComp,
                            AActor* Other,
                            class UPrimitiveComponent* OtherComp,
                            bool bSelfMoved,
                            FVector HitLocation,
                            FVector HitNormal,
                            FVector NormalImpulse,
                            const FHitResult & Hit) override;

};
