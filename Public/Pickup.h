#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

/**
*
*/
UCLASS()
class PROTOTYPE_API APickup : public AActor
{
    GENERATED_BODY()

public:
    // True when the pickup is possible, false if something deactivates the pickup
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
    bool bIsActive;

    // Simple collision primitive to use as the root component
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    USphereComponent* BaseCollisionComponent;

    // Static mesh component to represent the pickup in the level
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
    UStaticMeshComponent* PickupMesh;

    // Function to call when the Pickup is collected
    UFUNCTION(BlueprintNativeEvent)
    void OnPickedUp();

    // Constructor
    APickup(const FObjectInitializer& ObjectInitializer);
};
