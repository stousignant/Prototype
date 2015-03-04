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

    /** BoxComponent to specify the death area */
    UPROPERTY(VisibleInstanceOnly, Category = Gameplay)
    UBoxComponent* DeathFloorCollider;
	
};
