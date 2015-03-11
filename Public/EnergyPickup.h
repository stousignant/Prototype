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

    // Static mesh component to represent the beam in the level
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    UStaticMeshComponent* BeamMesh;

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

    // Override the OnPickedUp function
    void OnPickedUp_Implementation() override;
	
    // Happens when the energy hit the deathfloor
    void Explode();

    /** Explosion particle effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
    UParticleSystem* ExplosionEffect;

    /** Sound to play when destroyed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* ExplosionSound;

};
