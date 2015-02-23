#include "Prototype.h"
#include "Pickup.h"

APickup::APickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Pickup is valid when created
    bIsActive = true;

    // Create the root SphereComponent to handle the pickup's collision
    BaseCollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));

    // Set the SphereComponent as the root component
    RootComponent = BaseCollisionComponent;

    // Create the static mesh component
    PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PickupMesh"));

    // Turn physics on for the static mesh
    PickupMesh->SetSimulatePhysics(true);

    // Attach the Static mesh component to the root component
    PickupMesh->AttachTo(RootComponent);

}

void APickup::OnPickedUp_Implementation() //<--this is a Blueprint Native event, so we need to add _Implementation
{
    // There is no default behaviour for a Pickup when it is picked up
}


