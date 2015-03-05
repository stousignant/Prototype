#pragma once

#include "GameFramework/Actor.h"
#include "DynamicEnvAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API ADynamicEnvAsset : public AActor
{
    GENERATED_BODY()

public:
    // Constructor
    ADynamicEnvAsset(const FObjectInitializer& ObjectInitializer);

    // Update
    virtual void Tick(float DeltaSeconds) override;

    /** Particles */
    UPROPERTY(VisibleAnywhere, Category = Particle)
    UParticleSystem* SmokeParticle;
	
	
	
	
};
