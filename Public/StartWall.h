#pragma once

#include "GameFramework/Actor.h"
#include "StartWall.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AStartWall : public AActor
{
	GENERATED_BODY()

public:
    // Constructor
    AStartWall(const FObjectInitializer& ObjectInitializer);

    // Update
    virtual void Tick(float DeltaSeconds) override;

    /** BoxComponent to specify the start area */
    UPROPERTY(VisibleInstanceOnly, Category = Gameplay)
    UBoxComponent* StartWallCollider;	

    /** Enable-Disable the startwall */
    bool bIsEnabled;

};
