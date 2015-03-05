#include "Prototype.h"
#include "DynamicEnvAsset.h"

ADynamicEnvAsset::ADynamicEnvAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Create the simple StaticMeshComponent to represent the death floor
    SmokeParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystem>(this, TEXT("Particle"));
    
    // Make the actor tickable
    PrimaryActorTick.bCanEverTick = true;
}

void ADynamicEnvAsset::Tick(float DeltaSeconds)
{

}





