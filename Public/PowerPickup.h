#pragma once

#include "Pickup.h"
#include "PowerPickup.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API APowerPickup : public APickup
{
	GENERATED_BODY()
	
public:
    // Constructor
    APowerPickup(const FObjectInitializer& ObjectInitializer);

    //////////////////////////////////////////////////////////////////////////
    // Update
    /** Tick */
    virtual void Tick(float DeltaSeconds) override;
    
    /** Override the OnPickedUp function */
    void OnPickedUp_Implementation() override;

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
