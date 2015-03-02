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

    //////////////////////////////////////////////////////////////////////////
    // Update
    /** Tick */
    virtual void Tick(float DeltaSeconds) override;

    // Speed parameter of the energy
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float SpeedLevel;

    // Override the OnPickedUp function
    void OnPickedUp_Implementation() override;
	
    // Happens when the energy hit the deathfloor
    void Explode();

    /** Explosion particle effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
    UParticleSystem* ExplosionEffect;

};
