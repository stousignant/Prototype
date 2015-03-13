#pragma once

#include "GameFramework/Actor.h"
#include "DeathFloor.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API ADeathFloor : public AActor
{
	GENERATED_BODY()

public:
    // Constructor
    ADeathFloor(const FObjectInitializer& ObjectInitializer);
	
    // Update
    virtual void Tick(float DeltaSeconds) override;

    // Timer delay
    float TimerDelay;

    /** BoxComponent to specify the death area */
    UPROPERTY(VisibleInstanceOnly, Category = Gameplay)
    UBoxComponent* DeathFloorCollider;

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
