// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype.h"
#include "PrototypeCharacter.h"
#include "PrototypeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Engine.h"
#include "EnergyPickup.h"
#include "PowerPickup.h"
#include "PrototypeGameMode.h"

// Debug example
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GetActorRotation (%f,%f,%f)"), GetActorRotation().Vector().X, GetActorRotation().Vector().Y, GetActorRotation().Vector().Z));
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

const int MAX_LEVEL = 4;

//////////////////////////////////////////////////////////////////////////
// APrototypeCharacter
APrototypeCharacter::APrototypeCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(63.0f, 144.0f);

    // Create battery collection volume
    CollectionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
    CollectionSphere->AttachTo(RootComponent); // in order to follow our character
    CollectionSphere->SetSphereRadius(250.f);

	// Set turn rates for input
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

    // Set movement variables
    bIsRunning = false;
    bIsDashing = false;
    bIsWallRiding = false;
    bMovementBlocked = false;
    bIsFast = false;
    StaminaCurrent = StaminaMax;

    // Set default power values
    RunSpeed = 1500.0f;
    CharacterMovement->JumpZVelocity = 1500.0f;
    StaminaDrain = 1.0f;
    
    // Set scan variables
    bIsScanning = false;
    ScanMaximum = 180.0f;
    EnergyCount = 0.0f;
    ScanOffset1 = 100.0f;
    ScanOffset2 = 10.0f;

    // Set power variables
    SpeedPowerLevel = 1.0f;
    JumpPowerLevel = 1.0f;
    StaminaPowerLevel = 1.0f;

    // Set misc variables
    WindControlModifier = 4000;
    bIsDead = false;

    // Position of the camera on the character
    CameraRelativePosition = FVector(0, 0, 130);

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
    FirstPersonCameraComponent->RelativeLocation = CameraRelativePosition; // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
    
    // Distance of the scan
    ScanDistance = 5000.f;

    // Ten seconds renews respawn position
    RespawnSaveDelay = 600.f;
    // Store a first respawn position when created
    RespawnSaveTimer = RespawnSaveDelay;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void APrototypeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    // Manage the dash
    UpdateDash();

    // Manage the stamina
    UpdateStamina();

    // Manage wall riding
    UpdateWallRide(DeltaSeconds);

    // Manage respawn position
    UpdateRespawnPoint();

    // Manage the scanning
    UpdateScan();
        
    // * TEST DEBUG *
    // Shows a debug line of the input vector
    FVector AimStartingPoint = GetActorLocation() + CameraRelativePosition;
    FVector AimEndingPoint = FirstPersonCameraComponent->GetComponentRotation().Vector() * ScanDistance;
    //FVector AimEndingPoint = Controller->GetControlRotation().Vector() * ScanDistance; // Same thing
    //DrawDebugLine(GetWorld(), AimStartingPoint + FVector(0, 0, -5), AimEndingPoint, FColor(255, 255, 255));

    /*
    FVector AimStart;
    FRotator AimRotation;

    GetActorEyesViewPoint(AimStart, AimRotation);

    AimStart += FVector(0, 0, -5);

    FVector AimEnd = AimStart + AimRotation.Vector() * ScanDistance;

    DrawDebugLine(GetWorld(), AimStart, AimEnd, FColor::White);
    */
}

void APrototypeCharacter::UpdateDash()
{
    if (bIsDashing)
    {
        // Add movement in that direction
        AddMovementInput(GetActorForwardVector(), 1.f);
    }
    
    // Entered high speed
    if (GetCharacterMovement()->Velocity.Size() > WindControlModifier && !bIsFast)
    {
        bIsFast = true;
        UpdateWindSounds(bIsFast);

        // Get the character
        //APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
        //MyCharacter->Motion

        //struct FPostProcessSettings PostProcessSettings;
        //PostProcessSettings.MotionBlurAmount = 1.f;

    }
    // Exited high speed
    else if (GetCharacterMovement()->Velocity.Size() < WindControlModifier && bIsFast)
    {
        bIsFast = false;
        UpdateWindSounds(bIsFast);
    }
}

void APrototypeCharacter::UpdateStamina()
{
    if (bIsDashing)
    {
        // Remove stamina
        StaminaCurrent = (StaminaCurrent - StaminaDrain < 0) ? 0 : StaminaCurrent - StaminaDrain;

        if (StaminaCurrent <= 0)
        {
            bIsDashing = false;
            SetRunning(bIsRunning);
        }
    }
    // If the current stamina isn't maxed
    else if (StaminaCurrent < StaminaMax)
    {
        if (bIsRunning)
        {
            // Add stamina
            StaminaCurrent = (StaminaCurrent + StaminaReplenish > StaminaMax) ? StaminaMax : StaminaCurrent + StaminaReplenish;            
        }
        else
        {
            // Add stamina
            StaminaCurrent = (StaminaCurrent + StaminaReplenish * 2 > StaminaMax) ? StaminaMax : StaminaCurrent + StaminaReplenish * 2;
        }
    }    
}

void APrototypeCharacter::UpdateWallRide(float DeltaSeconds)
{
    FHitResult testHitResult(ForceInit);

    FCollisionQueryParams TraceParams(TEXT("MyTrace"), true, this);
    TraceParams.bTraceComplex = true;
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = false;

    // Perform a trace from the player's position to the input direction
    if (GetWorld()->LineTraceSingle(testHitResult, GetActorLocation(), GetActorLocation() + GetCharacterMovement()->GetLastInputVector() * 100, ECC_WorldStatic, TraceParams))
    {
        // If the trace has hit a wall
        if (testHitResult.GetActor() && testHitResult.GetComponent()->GetMaterial(0) == WallJumpMaterial)
        {
            // When falling
            if (GetCharacterMovement()->Velocity.Z < 0)
            {
                bIsWallRiding = true;

                // Add a force to counter gravity and to ride along the wall
                GetCharacterMovement()->AddForce(FVector(0, 0, (-1 * (GetVelocity().Z / DeltaSeconds)) * GetCapsuleComponent()->GetMass() / 10));

                UpdateWallRideSounds(true);
            }
            else
            {
                bIsWallRiding = false;
            }
        }
        else
        {
            bIsWallRiding = false;
        }
    }
    else
    {
        bIsWallRiding = false;
    }

    if (!bIsWallRiding)
    {
        UpdateWallRideSounds(false);
    }
}

void APrototypeCharacter::UpdateRespawnPoint()
{
    RespawnSaveTimer++;
    if (RespawnSaveTimer >= RespawnSaveDelay && !GetCharacterMovement()->IsFalling())
    {
        RespawnPosition = GetActorLocation();
        RespawnSaveTimer = 0.f;
    }
}

void APrototypeCharacter::UpdateScan()
{
    // Skip the scan update if player isn't pressing the scan input
    if (!bWantsToScan) return;

    // Get the aim starting and ending positions
    FVector AimStart;
    FRotator AimRotation;
    GetActorEyesViewPoint(AimStart, AimRotation);
    FVector AimEnd = AimStart + AimRotation.Vector() * ScanDistance;

    ULineBatchComponent* const LineBatcher = GetWorld()->ForegroundLineBatcher;

    // Trace the line for scan
    LineBatcher->DrawLine(AimStart + AimRotation.Vector().UpVector * ScanOffset1 + GetActorRightVector() * ScanOffset1, AimEnd + AimRotation.Vector().UpVector * ScanOffset2 + GetActorRightVector() * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Top right
    LineBatcher->DrawLine(AimStart - AimRotation.Vector().UpVector * ScanOffset1 + GetActorRightVector() * ScanOffset1, AimEnd - AimRotation.Vector().UpVector * ScanOffset2 + GetActorRightVector() * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Bottom right
    LineBatcher->DrawLine(AimStart - AimRotation.Vector().UpVector * ScanOffset1 - GetActorRightVector() * ScanOffset1, AimEnd - AimRotation.Vector().UpVector * ScanOffset2 - GetActorRightVector() * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Bottom left
    LineBatcher->DrawLine(AimStart + AimRotation.Vector().UpVector * ScanOffset1 - GetActorRightVector() * ScanOffset1, AimEnd + AimRotation.Vector().UpVector * ScanOffset2 - GetActorRightVector() * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Top left

    // Set the params for the raycast
    FHitResult testHitResult(ForceInit);
    FCollisionQueryParams TraceParams(TEXT("MyTrace"), true, this);
    TraceParams.bTraceComplex = true;
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = false;

    //FVector AimStartingPoint = GetActorLocation() + CameraRelativePosition;
    //FVector AimEndingPoint = FirstPersonCameraComponent->GetComponentRotation().Vector() * ScanDistance;

    // Perform a trace from the player's aim
    if (GetWorld()->LineTraceSingle(testHitResult, AimStart, AimEnd, ECC_WorldStatic, TraceParams))
    {
        // If the trace has hit an energy sphere
        bIsScanning = (testHitResult.GetActor() && testHitResult.GetActor()->GetClass()->IsChildOf(AEnergyPickup::StaticClass()));
    }
    else
    {
        bIsScanning = false;
    }

    // Progress of the scan
    if (bIsScanning)
    {
        ScanProgress++;

        // Scan completed
        if (ScanProgress >= ScanMaximum)
        {
            // Call the on picked up function
            AEnergyPickup* const TestEnergy = Cast<AEnergyPickup>(testHitResult.GetActor());
            TestEnergy->OnPickedUp();

            // Play energy sound
            UGameplayStatics::PlaySoundAtLocation(this, ScanSound, GetActorLocation());
        }
    }
    else
    {
        ScanProgress = 0;
    }
}


//////////////////////////////////////////////////////////////////////////
// Input
void APrototypeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	//InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    InputComponent->BindAction("Jump", IE_Pressed, this, &APrototypeCharacter::OnJumpPressed);
    InputComponent->BindAction("Jump", IE_Released, this, &APrototypeCharacter::OnJumpReleased);

    InputComponent->BindAction("Run", IE_Pressed, this, &APrototypeCharacter::OnRunPressed);
    InputComponent->BindAction("Run", IE_Released, this, &APrototypeCharacter::OnRunReleased);

    InputComponent->BindAction("Dash", IE_Pressed, this, &APrototypeCharacter::OnDashPressed);
    InputComponent->BindAction("Dash", IE_Released, this, &APrototypeCharacter::OnDashReleased);

    InputComponent->BindAction("Dive", IE_Pressed, this, &APrototypeCharacter::OnDivePressed);

    InputComponent->BindAction("Crouch", IE_Pressed, this, &APrototypeCharacter::OnCrouchPressed);

    InputComponent->BindAction("Scan", IE_Pressed, this, &APrototypeCharacter::OnScanPressed);
    InputComponent->BindAction("Scan", IE_Released, this, &APrototypeCharacter::OnScanReleased);

    InputComponent->BindAction("UpgradePower1", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower1Pressed);
    InputComponent->BindAction("UpgradePower2", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower2Pressed);
    InputComponent->BindAction("UpgradePower3", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower3Pressed);

    InputComponent->BindAction("ExitGame", IE_Pressed, this, &APrototypeCharacter::OnExitGamePressed);
    InputComponent->BindAction("RestartGame", IE_Pressed, this, &APrototypeCharacter::OnRestartGamePressed);
	
	InputComponent->BindAxis("MoveForward", this, &APrototypeCharacter::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &APrototypeCharacter::OnMoveRight);
	
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APrototypeCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APrototypeCharacter::LookUpAtRate);
}

void APrototypeCharacter::OnMoveForward(float Value)
{
    if (Value != 0.0f && !bMovementBlocked)
	{
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APrototypeCharacter::OnMoveRight(float Value)
{
    if (Value != 0.0f && !bMovementBlocked)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APrototypeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APrototypeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APrototypeCharacter::OnJumpPressed()
{
    if (!bMovementBlocked)
    {
        // If character is currently falling, check walls
        if (GetCharacterMovement()->IsFalling())
        {
            FHitResult testHitResult(ForceInit);

            FCollisionQueryParams TraceParams(TEXT("MyTrace"), true, this);
            TraceParams.bTraceComplex = true;
            TraceParams.bTraceAsyncScene = true;
            TraceParams.bReturnPhysicalMaterial = false;

            if (GetWorld()->LineTraceSingle(testHitResult, GetActorLocation(), GetActorLocation() + GetCharacterMovement()->GetLastInputVector() * 100, ECC_WorldStatic, TraceParams))
            {
                if (testHitResult.GetActor() && testHitResult.GetComponent()->GetMaterial(0) == WallJumpMaterial)
                {
                    bPressedJump = true;
                }
            }
        }
        // If is dashing
        else if (GetCharacterMovement()->Velocity.Size() >= RunSpeed * 1.25f)
        {
            // Verify if enough stamina
            if (StaminaCurrent - StaminaDrain * 50 > 0)
            {
                // Execute jump action
                bPressedJump = true;

                // Drain stamina from dash jumping
                StaminaCurrent = (StaminaCurrent - StaminaDrain * 50 < 0) ? 0 : StaminaCurrent - StaminaDrain * 50;
            }
        }
        else
        {
            // Execute jump action
            bPressedJump = true;
        }

        // If jumped
        if (bPressedJump)
        {
            // Play jump sound
            UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
        }
    }    
}

void APrototypeCharacter::OnJumpReleased()
{
    bPressedJump = false;
}

void APrototypeCharacter::OnRunPressed()
{
    // Set the running to true
    SetRunning(true);
}

void APrototypeCharacter::OnRunReleased()
{
    // Set the running to false
    SetRunning(false);
}

void APrototypeCharacter::SetRunning(bool bNewRunning)
{
    // If character is not dashing
    if (!bIsDashing)
    {
        // Assign new value to the run
        bIsRunning = bNewRunning;

        // Changing the character's movement speed
        GetCharacterMovement()->MaxWalkSpeed = (bIsRunning) ? RunSpeed : RunSpeed / 2;
        GetCharacterMovement()->MaxAcceleration = NormalAccel;
    }

    // Make sure the character is grounded
    if (!GetCharacterMovement()->IsFalling())
    {
        // Play run sound
        UpdateRunSounds(bNewRunning);
    }
}

void APrototypeCharacter::OnDashPressed()
{
    if (!bMovementBlocked)
    {
        if (GetCharacterMovement()->IsFalling())
        {
            // Verify if enough stamina
            if (StaminaCurrent - StaminaDrain * 50 < 0)
            {
                // Cancel dash
                return;
            }
            else
            {
                // Drain stamina from dash jumping
                StaminaCurrent = (StaminaCurrent - StaminaDrain * 50 < 0) ? 0 : StaminaCurrent - StaminaDrain * 50;
            }            
        }

        bIsRunning = false;

        // Stop the run sound
        UpdateRunSounds(false);

        bIsDashing = true;        

        // Changing the character's movement speed & acceleration
        GetCharacterMovement()->MaxWalkSpeed = RunSpeed * 2;
        GetCharacterMovement()->MaxAcceleration = DashAccel;           
    }
}

void APrototypeCharacter::OnDashReleased()
{
    bIsDashing = false;

    SetRunning(bIsRunning);
}

void APrototypeCharacter::OnDivePressed()
{
    // Verify if enough stamina
    if (StaminaCurrent - StaminaDrain * 25 < 0)
    {
        // Cancel dive
        return;
    }
    else
    {
        // Drain stamina from dash jumping
        StaminaCurrent = (StaminaCurrent - StaminaDrain * 25 < 0) ? 0 : StaminaCurrent - StaminaDrain * 25;
    }

    // Higher the fall speed
    GetPawnPhysicsVolume()->TerminalVelocity = DiveSpeed;

    // Add a force downward
    GetCharacterMovement()->Velocity += FVector(0, 0, -10000);
}

void APrototypeCharacter::OnCrouchPressed()
{
    GetCharacterMovement()->bWantsToCrouch = !GetCharacterMovement()->IsCrouching();

    if (GetCharacterMovement()->bWantsToCrouch && GetCharacterMovement()->Velocity.Size() > (RunSpeed / 2))
    {
        // Block movement in order to perform a slide
        bMovementBlocked = true;

        // Change the ground friction in order to simulate a sliding effect
        GetCharacterMovement()->GroundFriction = 0.5f;

        // Give the character a small boost
        GetCharacterMovement()->Velocity += FVector(GetActorRotation().Vector().X * RunSpeed, GetActorRotation().Vector().Y * RunSpeed, 0);
    }
    else
    {
        // Unblock movement
        bMovementBlocked = false;

        // Set the ground friction back to default
        GetCharacterMovement()->GroundFriction = 8.f;
    }
}

void APrototypeCharacter::OnScanPressed()
{
    bWantsToScan = true;
}

void APrototypeCharacter::OnScanReleased()
{
    bWantsToScan = false;
}

void APrototypeCharacter::OnUpgradePower1Pressed()
{
    UpgradePower(SpeedPowerLevel);

    if (SpeedPowerLevel == 2)
    {
        RunSpeed = 2000.0f;
    }
    else if (SpeedPowerLevel == 3)
    {
        RunSpeed = 2500.0f;
    }
    else if (SpeedPowerLevel == 4)
    {
        RunSpeed = 3000.0f;
    }

    // Update speed
    GetCharacterMovement()->MaxWalkSpeed = (bIsRunning) ? RunSpeed : RunSpeed / 2;
}

void APrototypeCharacter::OnUpgradePower2Pressed()
{
    UpgradePower(JumpPowerLevel);

    if (JumpPowerLevel == 2)
    {
        CharacterMovement->JumpZVelocity = 2250.0f;
    }
    else if (JumpPowerLevel == 3)
    {
        CharacterMovement->JumpZVelocity = 3000.0f;
    }
    else if (JumpPowerLevel == 4)
    {
        CharacterMovement->JumpZVelocity = 3750.0f;
    }
}

void APrototypeCharacter::OnUpgradePower3Pressed()
{
    UpgradePower(StaminaPowerLevel);

    if (StaminaPowerLevel == 2)
    {
        StaminaDrain = 0.75f;
    }
    else if (StaminaPowerLevel == 3)
    {
        StaminaDrain = 0.5f;
    }
    else if (StaminaPowerLevel == 4)
    {
        StaminaDrain = 0.25f;
    }
}

void APrototypeCharacter::UpgradePower(float &PowerId)
{
    if (StatsCount > 0 && PowerId < MAX_LEVEL)
    {
        PowerId++;
        StatsCount--;

        // Play upgrade sound
        UGameplayStatics::PlaySoundAtLocation(this, PowerUpgradeSound, GetActorLocation());
    }
}

void APrototypeCharacter::OnExitGamePressed()
{
    GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void APrototypeCharacter::OnRestartGamePressed()
{
    GetWorld()->GetFirstPlayerController()->ConsoleCommand("restartlevel");
}


//////////////////////////////////////////////////////////////////////////
// Base Movement Override
void APrototypeCharacter::OnLanded(const FHitResult& Hit)
{
    // Change the fall speed to normal
    GetPawnPhysicsVolume()->TerminalVelocity = FallSpeed;

    // Parent call
    Super::OnLanded(Hit);

    // Play land sound
    UGameplayStatics::PlaySoundAtLocation(this, LandSound, GetActorLocation(), -1 * GetCharacterMovement()->Velocity.Z / FallSpeed);

    // If player is currently holding shift
    if (bIsRunning)
    {
        // Play run sound
        UpdateRunSounds(true);
    }
}

void APrototypeCharacter::Falling()
{
    // Stop run sound when falling
    UpdateRunSounds(false);
}

bool APrototypeCharacter::CanJumpInternal_Implementation() const
{
    return true;
}


//////////////////////////////////////////////////////////////////////////
// Sound
void APrototypeCharacter::UpdateRunSounds(bool bNewRunning)
{
    if (bNewRunning)
    {
        // If the run audio component isn't created yet
        if (!RunAC && RunSound)
        {
            // Initialize the run audio component
            RunAC = UGameplayStatics::PlaySoundAttached(RunSound, GetRootComponent());
            if (RunAC)
            {
                RunAC->bAutoDestroy = false;
            }
        }
        // If the run audio component exists
        else if (RunAC)
        {
            // Play the run sound
            RunAC->Play();
        }
    }
    else
    {
        // If the run audio component exists 
        if (RunAC)
        {
            // Stop the run sound
            RunAC->Stop();
        }
    }
}

void APrototypeCharacter::UpdateWindSounds(bool bNewIsFast)
{
    if (bNewIsFast)
    {
        // If the wind audio component isn't created yet
        if (!WindAC && WindSound)
        {
            // Initialize the run audio component
            WindAC = UGameplayStatics::PlaySoundAttached(WindSound, GetRootComponent());
            if (WindAC)
            {
                WindAC->bAutoDestroy = false;
            }
        }
        else if (WindAC)
        {
            WindAC->Play();
        }
    }
    else
    {
        if (WindAC)
        {
            WindAC->Stop();
        }
    }
}

void APrototypeCharacter::UpdateWallRideSounds(bool bNewIsWallRiding)
{
    if (bNewIsWallRiding)
    {
        // If the wall ride audio component isn't created yet
        if (!WallRideAC && WallRideSound)
        {
            // Initialize the run audio component
            WallRideAC = UGameplayStatics::PlaySoundAttached(WallRideSound, GetRootComponent());
            if (WallRideAC)
            {
                WallRideAC->bAutoDestroy = false;
            }
        }
        else if (WallRideAC && !WallRideAC->IsPlaying())
        {
            WallRideAC->Play();
        }
    }
    else
    {
        if (WallRideAC)
        {
            WallRideAC->Stop();
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// Power
/*void APrototypeCharacter::CollectPowerup()
{
    // Get all overlapping Actors and store them in a CollectedActors array
    TArray<AActor*> CollectedActors;
    CollectionSphere->GetOverlappingActors(CollectedActors);

    // For each actor collected
    for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
    {
        // Cast the collected actor to APowerPickup
        APowerPickup* const TestPowerup = Cast<APowerPickup>(CollectedActors[iCollected]);

        // if the cast is successful, and the power up is valid and active
        if (TestPowerup && !TestPowerup->IsPendingKill() && TestPowerup->bIsActive)
        {
            // Call the battery's OnPickedUp function
            TestPowerup->OnPickedUp();

            // Deactivate the battery
            TestPowerup->bIsActive = false;

            // Add a stat to the player
            StatsCount++;
        }
    }
}*/


//////////////////////////////////////////////////////////////////////////
// Spawning
void APrototypeCharacter::Die()
{
    // TODO :: Fade in/out to white

    // Set death state to true
    bIsDead = true;

    // Remove any velocity
    GetCharacterMovement()->Velocity = FVector(0,0,0);

    // Play death sound
    UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    
    // Finish the respawn with a 2 second delay
    FTimerHandle UniqueHandle;
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &APrototypeCharacter::Respawn);
    GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, 2.f, false);
}

void APrototypeCharacter::Respawn()
{
    // Change the fall speed back to normal
    GetPawnPhysicsVolume()->TerminalVelocity = FallSpeed;

    // Position the player
    SetActorLocation(RespawnPosition + FVector(0, 0, 1000));

    // Play respawn sound
    UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());

    // Set death state to false
    bIsDead = false;
}


//////////////////////////////////////////////////////////////////////////
// Misc

