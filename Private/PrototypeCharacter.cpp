#include "Prototype.h"
#include "PrototypeCharacter.h"
#include "PrototypeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Engine.h"
#include "EnergyPickup.h"
#include "PowerPickup.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"

// Debug example
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("testHitResult.GetComponent() (%s)"), *testHitResult.GetComponent()->GetName()));
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GetActorRotation (%f)"), GetActorRotation().Vector().X));
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));

const int MAX_LEVEL = 15;
const float MAX_EXPERIENCE = 100.0f;

//////////////////////////////////////////////////////////////////////////
// APrototypeCharacter
APrototypeCharacter::APrototypeCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(63.0f, 144.0f);

    // Create battery collection volume
    CollectionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
    CollectionSphere->AttachTo(RootComponent); // in order to follow our character
    CollectionSphere->SetSphereRadius(5000.f);

	// Set turn rates for input
	BaseTurnRate = 15.0f;
	BaseLookUpRate = 15.0f;

    // Set scan variables
    bIsScanning = false;
    EnergyCount = 0.0f;
    ScanOffset1 = 100.0f;
    ScanOffset2 = 10.0f;

    // Set power variables
    SpeedPowerLevel = 1.0f;
    SpeedPowerDefault = 1500.0f;
    SpeedPowerIncrement = 150.0f;
    AccelerationPowerDefault = 3000.0f;
    AccelerationPowerIncrement = 500.0f;

    JumpPowerLevel = 1.0f;
    JumpPowerDefault = 1800.0f;
    JumpPowerIncrement = 125.0f;

    StaminaPowerLevel = 1.0f;
    StaminaPowerDefault = 1.0f;
    StaminaPowerIncrement = 0.05f;

    AbsorbPowerLevel = 1.0f;
    AbsorbPowerTimeDefault = 4.0f; // 4 secs
    AbsorbPowerTime = AbsorbPowerTimeDefault;
    AbsorbPowerTimeIncrement = 0.5f;
    AbsorbPowerDistanceDefault = 2000.0f;
    AbsorbPowerDistance = AbsorbPowerDistanceDefault;
    AbsorbPowerDistanceIncrement = 500.0f;

    PowerupDetectionTimer = 0.0f;

    // Set experience variables
    ExperiencePoints = 0.0f;
    ExperiencePointsNewlyGained = 0.0f;

    // Set default power values
    RunSpeed = SpeedPowerDefault;
    GetCharacterMovement()->MaxWalkSpeedCrouched = RunSpeed / 3;
    GetCharacterMovement()->JumpZVelocity = JumpPowerDefault;
    StaminaDrain = StaminaPowerDefault;

    // Overload
    bOverloadStarted = false;
    bOverloadEnded = false;
    bIsOverloaded = false;
    OverloadCooldownDuration = 60.0f; // 1 min
    OverloadCooldownTimer = 0.0f;
    OverloadXPDrainDefault = 5.0f; // 20secs for 100xp
    OverloadXPDrain = OverloadXPDrainDefault;
    OverloadXPDrainTimeWarp = 10.0f; // 10secs for 100xp with TimeWarp
    bWantsToTimeWarp = false;

    // Set movement variables
    bIsRunning = false;
    bIsDashing = false;
    bIsWallRiding = false;
    bMovementBlocked = false;
    bIsFast = false;
    StaminaMax = 100.0f;
    StaminaCurrent = StaminaMax;
    StaminaReplenish = 0.1f;
    NormalAccel = AccelerationPowerDefault;
    WallRideDistance = 1000.0f;
    SlideStaminaConsumption = 30.0f;
    DiveStaminaConsumption = 30.0f;
    DashJumpStaminaConsumption = 60.0f;

    // Set misc variables
    WindControlModifier = 3999;
    bIsDead = false;

    // Position of the camera on the character
    CameraRelativePosition = FVector(0, 0, 130);

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
    FirstPersonCameraComponent->RelativeLocation = CameraRelativePosition; // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
    
    // Ten seconds renews respawn position
    RespawnSaveNormalDelay = 10.f;
    RespawnSaveOverloadDelay = 1.f;
    // Store a first respawn position when created
    RespawnSaveTimer = RespawnSaveNormalDelay;
    RespawnSaveFailnetTimer = RespawnSaveNormalDelay;
    RespawnNormalTime = 2.0f;
    RespawnOverloadTime = 0.5f;
    RespawnTime = RespawnNormalTime;

    // For Land volume multiplier
    DiveSpeed = 0.0f;

    // Pause variable
    bIsGamePaused = false;

    // Hack variable
    bCanHack = false;
}

//////////////////////////////////////////////////////////////////////////
// Tick (called 60 times per second due to the 60 fps limit)
void APrototypeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    // Manage the run
    UpdateRun(DeltaSeconds);

    // Manage the dash
    UpdateDash(DeltaSeconds);

    // Manage the stamina
    UpdateStamina(DeltaSeconds);

    // Manage wall riding
    UpdateWallRide(DeltaSeconds);

    // Manage respawn position
    UpdateRespawnPoint(DeltaSeconds);

    // Manage the scanning
    UpdateScan(DeltaSeconds);

    // Manage the slide
    UpdateSlide(DeltaSeconds);

    // Manage the powerup detection
    UpdatePowerupDetection(DeltaSeconds);
    
    // Manage the wind speed sound
    UpdateSound(DeltaSeconds);

    // Manage the overload
    UpdateOverload(DeltaSeconds);
}

void APrototypeCharacter::UpdateRun(float DeltaSeconds)
{
    if (bIsRunning)
    {
        // Make sure the character is grounded and is moving
        if (!GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Size() > WalkSpeed && !bIsDashing && !bIsSliding && !bIsCrouched)
        {
            // Play run sound
            UpdateRunSounds(true);
        }
        else
        {
            // Stop run sound
            UpdateRunSounds(false);
        }
    }
}

void APrototypeCharacter::UpdateDash(float DeltaSeconds)
{
    if (bIsDashing)
    {
        // Add movement in that direction
        AddMovementInput(GetActorForwardVector(), 1.f);

        UpdateOverloadDashSounds(!GetCharacterMovement()->IsFalling() && bIsOverloaded);
    }
    else
    {
        UpdateOverloadDashSounds(false);
    }
    
    // Entered high speed
    if (GetCharacterMovement()->Velocity.Size() > WindControlModifier && !bIsFast)
    {
        bIsFast = true;
        UpdateWindSounds(bIsFast);
    }
    // Exited high speed
    else if (GetCharacterMovement()->Velocity.Size() < WindControlModifier && bIsFast)
    {
        bIsFast = false;
        UpdateWindSounds(bIsFast);
    }
}

void APrototypeCharacter::UpdateStamina(float DeltaSeconds)
{
    if (bIsDashing)
    {
        // Remove stamina
        float StaminaDrainWithDelta = StaminaDrain * DeltaSeconds * 60.0f / 2.0f;
        StaminaCurrent = (StaminaCurrent - StaminaDrainWithDelta < 0) ? 0 : StaminaCurrent - StaminaDrainWithDelta;
        
        if (StaminaCurrent <= 0)
        {
            MissingStamina();

            bIsDashing = false;
            SetRunning(bIsRunning);
        }
    }
    // If the current stamina isn't maxed
    else if (StaminaCurrent < StaminaMax)
    {
        // Add stamina
        float StaminaReplenishWithDelta = StaminaReplenish * DeltaSeconds * 60.0f;

        if (bIsRunning)
        {
            StaminaCurrent = (StaminaCurrent + StaminaReplenishWithDelta > StaminaMax) ? StaminaMax : StaminaCurrent + StaminaReplenishWithDelta;
        }
        else if (bIsCrouched && !GetCharacterMovement()->IsFalling())
        {
            StaminaCurrent = (StaminaCurrent + StaminaReplenishWithDelta * 2 > StaminaMax) ? StaminaMax : StaminaCurrent + StaminaReplenishWithDelta * 4;
        }
        else
        {
            StaminaCurrent = (StaminaCurrent + StaminaReplenishWithDelta * 2 > StaminaMax) ? StaminaMax : StaminaCurrent + StaminaReplenishWithDelta * 2;
        }
    }
    
    if (MissingStaminaTimer > 0.0f)
    {
        // Set missing stamina to true
        bIsMissingStamina = true;

        // Deduct time
        MissingStaminaTimer -= DeltaSeconds;
    }
    else
    {
        bIsMissingStamina = false;
    }

    // For hack
    if (StaminaCurrent > StaminaMax)
    {
        StaminaCurrent = StaminaMax;
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
    if (GetWorld()->LineTraceSingle(testHitResult, GetActorLocation(), GetActorLocation() + GetCharacterMovement()->GetLastInputVector() * WallRideDistance, ECC_WorldStatic, TraceParams))
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

void APrototypeCharacter::UpdateRespawnPoint(float DeltaSeconds)
{
    // Only update respawn point if in playing mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (MyGameMode->GetCurrentState() == EPrototypePlayState::ENormalMode ||
        MyGameMode->GetCurrentState() == EPrototypePlayState::EHardMode ||
        MyGameMode->GetCurrentState() == EPrototypePlayState::EVeryHardMode || 
        MyGameMode->GetCurrentState() == EPrototypePlayState::EUltimateMode)
    {
        RespawnSaveTimer += DeltaSeconds;

        float RespawnSaveDelay = (bIsOverloaded) ? RespawnSaveOverloadDelay : RespawnSaveNormalDelay;

        if (RespawnSaveTimer >= RespawnSaveDelay && !GetCharacterMovement()->IsFalling() && !bIsDead && !bIsCrouched)
        {
            // Ensure respawn point is safe by waiting 2.0 seconds when character is grounded
            RespawnSaveFailnetTimer += DeltaSeconds;

            if (RespawnSaveFailnetTimer >= 2.0f)
            {
                RespawnPosition = GetActorLocation();
                RespawnSaveTimer = 0.0f;
                RespawnSaveFailnetTimer = 0.0f;
            }            
        }
    }    
}

void APrototypeCharacter::UpdateScan(float DeltaSeconds)
{
    // Skip the scan update if player isn't pressing the scan input
    if (!bWantsToScan)
    {
        // Reset scan variables
        bIsScanning = false;
        ScanProgress = 0;
        return;
    }

    // Get the aim starting and ending positions
    FVector AimStart;
    FRotator AimRotation;
    GetActorEyesViewPoint(AimStart, AimRotation);
    FVector AimEnd = AimStart + AimRotation.Vector() * AbsorbPowerDistance;

    ULineBatchComponent* const LineBatcher = GetWorld()->ForegroundLineBatcher;

    // Trace the line for scan
    FVector RelativeUpVector = FirstPersonCameraComponent->GetUpVector();
    FVector RelativeRightVector = FirstPersonCameraComponent->GetRightVector();
    LineBatcher->DrawLine(AimStart + RelativeUpVector * ScanOffset1 +  RelativeRightVector * ScanOffset1, AimEnd + RelativeUpVector * ScanOffset2 + RelativeRightVector * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Top right
    LineBatcher->DrawLine(AimStart - RelativeUpVector * ScanOffset1 +  RelativeRightVector * ScanOffset1, AimEnd - RelativeUpVector * ScanOffset2 + RelativeRightVector * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Bottom right
    LineBatcher->DrawLine(AimStart - RelativeUpVector * ScanOffset1 -  RelativeRightVector * ScanOffset1, AimEnd - RelativeUpVector * ScanOffset2 - RelativeRightVector * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Bottom left
    LineBatcher->DrawLine(AimStart + RelativeUpVector * ScanOffset1 -  RelativeRightVector * ScanOffset1, AimEnd + RelativeUpVector * ScanOffset2 - RelativeRightVector * ScanOffset2, FColor::Cyan, 128, 10.0f, 1.0f); // Top left

    // Set the params for the raycast
    FHitResult testHitResult(ForceInit);
    FCollisionQueryParams TraceParams(TEXT("MyTrace"), true, this);
    TraceParams.bTraceComplex = true;
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bReturnPhysicalMaterial = false;
    
    // Perform a trace from the player's aim
    if (GetWorld()->LineTraceSingle(testHitResult, AimStart, AimEnd, ECC_WorldStatic, TraceParams))
    {
        // If the trace has hit an energy sphere
        bIsScanning = (testHitResult.GetActor() && testHitResult.GetActor()->GetClass()->IsChildOf(AEnergyPickup::StaticClass()));

        ScanDistance = FVector::Dist(GetActorLocation(), testHitResult.Location) / 200.0f;
    }
    else
    {
        bIsScanning = false;

        ScanDistance = 0.0f;
    }

    // Progress of the scan
    if (bIsScanning)
    {
        // If scan is hitting the energy
        if (testHitResult.GetComponent()->GetName() == "PickupMesh")
        {
            // Scanning center is x4 faster
            ScanProgress += DeltaSeconds * 4;
        }
        // If scan is hitting the beam
        else if (testHitResult.GetComponent()->GetName() == "BeamMesh")
        {
            ScanProgress += DeltaSeconds;
        }
        
        // Scan completed
        if (ScanProgress >= AbsorbPowerTime)
        {
            // Call the on picked up function
            AEnergyPickup* const TestEnergy = Cast<AEnergyPickup>(testHitResult.GetActor());
            TestEnergy->Absorb();

            // Get HUD
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
            APrototypeHUD* MyGameHUD = Cast<APrototypeHUD>(PlayerController->GetHUD());

            // Increment xp
            ExperiencePoints += TestEnergy->ExperiencePoints;
            ExperiencePointsNewlyGained = TestEnergy->ExperiencePoints;

            // If leveled up
            if (ExperiencePoints >= MAX_EXPERIENCE)
            {
                // If in overload
                if (bIsOverloaded)
                {
                    // Cap experience gain at 100.0f
                    ExperiencePoints = MAX_EXPERIENCE;

                    // Still show xp gain
                    MyGameHUD->bShowXPGain = true;
                }
                // If normal
                else
                {
                    // Level up
                    LevelUp(true);

                    // Show it
                    MyGameHUD->bShowLevelUp = true;

                    // Decrease xp if he didn't put his stat from leveling up
                    ExperiencePoints -= MAX_EXPERIENCE;
                }                            
            }
            else
            {
                // show xp gain
                MyGameHUD->bShowXPGain = true;
            }

            // Play energy sound
            UGameplayStatics::PlaySoundAtLocation(this, ScanSound, GetActorLocation());

            // Reset scan progress
            ScanProgress = 0.0f;
        }
    }
    else
    {
        ScanProgress = 0;
    }
}

void APrototypeCharacter::UpdateSlide(float DeltaSeconds)
{
    if (bIsSliding)
    {
        // Play the slide sound
        UpdateSlideSounds(true);
    }

    // If slide is over
    if (bIsSliding && bIsCrouched && GetCharacterMovement()->Velocity.Size() <= (RunSpeed / 2))
    {
        SetSliding(false);
    }
    // If slide is interrupted from releasing crouch
    else if (bIsSliding && !bIsCrouched)
    {
        SetSliding(false);
    }
}

void APrototypeCharacter::UpdatePowerupDetection(float DeltaSeconds)
{
    PowerupDetectionTimer += DeltaSeconds;

    // Execute detection each 0.5 second
    if (PowerupDetectionTimer > 0.5f)
    {
        PowerupDetectedCount = 0.0f;

        // Get all overlapping Actors and store them in a CollectedActors array
        TArray<AActor*> CollectedActors;
        CollectionSphere->GetOverlappingActors(CollectedActors);

        // For each actor collected
        for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
        {
            // Cast the collected actor to APowerPickup
            APowerPickup* const TestPowerup = Cast<APowerPickup>(CollectedActors[iCollected]);

            // if the cast is successful, and the powerup is valid and active
            if (TestPowerup && !TestPowerup->IsPendingKill() && TestPowerup->bIsActive)
            {
                PowerupDetectedCount++;
            }
        }
    }
}

void APrototypeCharacter::UpdateSound(float DeltaSeconds)
{
    if (WindAC && WindAC->IsPlaying())
    {
        WindAC->SetVolumeMultiplier(GetCharacterMovement()->Velocity.Size() / WindControlModifier);
    }
}

void APrototypeCharacter::UpdateOverload(float DeltaSeconds)
{
    if (OverloadCooldownTimer > 0.0f)
    {
        OverloadCooldownTimer -= DeltaSeconds;

        if (OverloadCooldownTimer < 0.0f)
        {
            // Cooldown is back
            OverloadCooldownTimer = 0.0f;
        }
    }

    if (bIsOverloaded)
    {
        if (bWantsToTimeWarp && !bIsTimeWarping)
        {
            bIsTimeWarping = true;

            // Make current energypickup slower x2
            TArray<AActor*> FoundEnergyPickupActors;

            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyPickup::StaticClass(), FoundEnergyPickupActors);

            for (auto Actor : FoundEnergyPickupActors)
            {
                AEnergyPickup* EnergyActor = Cast<AEnergyPickup>(Actor);
                // Slow its speed
                EnergyActor->SpeedLevel /= 2.0f;
            }

            // Increase XP drain
            OverloadXPDrain = OverloadXPDrainTimeWarp;

            // Update time warp sounds
            UpdateTimeWarpSounds(bIsTimeWarping);
        }

        // Don't consume XP if hacking
        if (bIsHackingExperience)
        {
            return;
        }

        if (ExperiencePoints - (DeltaSeconds * OverloadXPDrain) < 0.0f)
        {
            EndOverload();
        }
        else
        {
            // Decrease experience
            ExperiencePoints -= DeltaSeconds * OverloadXPDrain;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Input
void APrototypeCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

    InputComponent->BindAction("Jump", IE_Pressed, this, &APrototypeCharacter::OnJumpPressed);
    InputComponent->BindAction("Jump", IE_Released, this, &APrototypeCharacter::OnJumpReleased);

    InputComponent->BindAction("Run", IE_Pressed, this, &APrototypeCharacter::OnRunPressed);
    InputComponent->BindAction("Run", IE_Released, this, &APrototypeCharacter::OnRunReleased);

    InputComponent->BindAction("Dash", IE_Pressed, this, &APrototypeCharacter::OnDashPressed);
    InputComponent->BindAction("Dash", IE_Released, this, &APrototypeCharacter::OnDashReleased);

    InputComponent->BindAction("Dive", IE_Pressed, this, &APrototypeCharacter::OnDivePressed);

    InputComponent->BindAction("Crouch", IE_Pressed, this, &APrototypeCharacter::OnCrouchPressed);
    InputComponent->BindAction("Crouch", IE_Released, this, &APrototypeCharacter::OnCrouchReleased);

    InputComponent->BindAction("Scan", IE_Pressed, this, &APrototypeCharacter::OnScanPressed);
    InputComponent->BindAction("Scan", IE_Released, this, &APrototypeCharacter::OnScanReleased);

    InputComponent->BindAction("TimeWarp", IE_Pressed, this, &APrototypeCharacter::OnTimeWarpPressed);
    InputComponent->BindAction("TimeWarp", IE_Released, this, &APrototypeCharacter::OnTimeWarpReleased);

    InputComponent->BindAction("UpgradePower1", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower1Pressed);
    InputComponent->BindAction("UpgradePower2", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower2Pressed);
    InputComponent->BindAction("UpgradePower3", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower3Pressed);
    InputComponent->BindAction("UpgradePower4", IE_Pressed, this, &APrototypeCharacter::OnUpgradePower4Pressed);

    InputComponent->BindAction("ExitGame", IE_Pressed, this, &APrototypeCharacter::OnExitGamePressed);
    InputComponent->BindAction("RestartGame", IE_Pressed, this, &APrototypeCharacter::OnRestartGamePressed);
    InputComponent->BindAction("ContinueGame", IE_Pressed, this, &APrototypeCharacter::OnContinueGamePressed);
    FInputActionBinding& PauseToggle = InputComponent->BindAction("PauseGame", IE_Pressed, this, &APrototypeCharacter::OnPauseGamePressed);
    PauseToggle.bExecuteWhenPaused = true;

    InputComponent->BindAction("Hack1", IE_Pressed, this, &APrototypeCharacter::OnHack1Pressed);
    InputComponent->BindAction("Hack2", IE_Pressed, this, &APrototypeCharacter::OnHack2Pressed);
	
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
            if (StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption > 0)
            {  
                // Execute jump action
                bPressedJump = true;

                // Drain stamina from dash jumping
                StaminaCurrent = (StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption < 0) ? 0 : StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption;
            }
            else
            {
                MissingStamina();
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
            // Calculate land sound volume multiplier
            float JumpVolumeMultiplier = 1.0f + JumpPowerLevel * 0.0667f;

            if (bIsOverloaded)
            {
                // Play overload jump sound
                UGameplayStatics::PlaySoundAtLocation(this, OverloadJumpSound, GetActorLocation(), JumpVolumeMultiplier);
            }

            // Play jump sound
            UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation(), JumpVolumeMultiplier);
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

    // Stop the run sound
    UpdateRunSounds(false);
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
}

void APrototypeCharacter::OnDashPressed()
{
    if (!bMovementBlocked)
    {
        // Super jump
        if (GetCharacterMovement()->IsFalling())
        {
            // Verify if enough stamina
            if (StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption < 0)
            {
                MissingStamina();

                // Cancel dash
                return;
            }
            else
            {
                // Drain stamina from dash jumping
                StaminaCurrent = (StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption < 0) ? 0 : StaminaCurrent - StaminaDrain * DashJumpStaminaConsumption;
            }            
        }

        bIsDashing = true;        

        // Changing the character's movement speed & acceleration
        //GetCharacterMovement()->MaxWalkSpeed = RunSpeed * 2;
        GetCharacterMovement()->MaxWalkSpeed = (bIsWallRiding) ? RunSpeed * 4 : RunSpeed * 2;
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
    // If dead, cannot dive
    if (bIsDead){return;}

    // Verify if enough stamina
    if (StaminaCurrent - StaminaDrain * DiveStaminaConsumption < 0)
    {
        MissingStamina();

        // Cancel dive
        return;
    }
    else
    {
        // Drain stamina from dash jumping
        StaminaCurrent = (StaminaCurrent - StaminaDrain * DiveStaminaConsumption < 0) ? 0 : StaminaCurrent - StaminaDrain * DiveStaminaConsumption;
    }

    // Higher the fall speed
    GetPawnPhysicsVolume()->TerminalVelocity = RunSpeed * 5;

    // Add a force downward
    GetCharacterMovement()->Velocity += FVector(0, 0, RunSpeed * -5);

    // Fall speed
    DiveSpeed = GetCharacterMovement()->Velocity.Z * -1;
}

void APrototypeCharacter::OnCrouchPressed()
{
    // Crouch the character
    GetCharacterMovement()->bWantsToCrouch = true;
    bWantsToCrouch = true;

    /* Slide */
    // In order to slide, must press crouch, not be walking (1.9f), and grounded
    if (GetCharacterMovement()->bWantsToCrouch && GetCharacterMovement()->Velocity.Size() > (RunSpeed / 1.9f) && !GetCharacterMovement()->IsFalling())
    {
        // If wants to slide and can
        VerifySlide();
    }
    
}

void APrototypeCharacter::VerifySlide()
{
    // Verify if enough stamina
    if (StaminaCurrent - StaminaDrain * SlideStaminaConsumption < 0)
    {
        MissingStamina();

        // Cancel slide
        return;
    }
    else
    {
        // Drain stamina from slide
        StaminaCurrent = (StaminaCurrent - StaminaDrain * SlideStaminaConsumption < 0) ? 0 : StaminaCurrent - StaminaDrain * SlideStaminaConsumption;
    }

    SetSliding(true);
}

void APrototypeCharacter::OnCrouchReleased()
{
    // Uncrouch the character
    GetCharacterMovement()->bWantsToCrouch = false;

    // Don't want to uncrouch if released
    bWantsToCrouch = false;
}

void APrototypeCharacter::SetSliding(bool bNewSliding)
{
    // Set slide state
    bIsSliding = bNewSliding;

    if (bIsSliding)
    {
        // Block movement in order to perform a slide
        bMovementBlocked = true;

        // Change the ground friction in order to simulate a sliding effect
        GetCharacterMovement()->GroundFriction = 0.25f;

        // Give the character a small boost
        GetCharacterMovement()->Velocity += FVector(GetActorRotation().Vector().X * RunSpeed, GetActorRotation().Vector().Y * RunSpeed, 0);

        // Stop the run sound
        UpdateRunSounds(false);
    }
    else
    {
        // Unblock movement
        bMovementBlocked = false;

        // Set the ground friction back to default
        GetCharacterMovement()->GroundFriction = 8.f;

        // Stop the slide sound
        UpdateSlideSounds(false);

        // Uncrouch if needed
        if (!bWantsToCrouch)
        {
            // Uncrouch the character
            GetCharacterMovement()->bWantsToCrouch = false;
        }
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

void APrototypeCharacter::OnTimeWarpPressed()
{
    bWantsToTimeWarp = true;
}

void APrototypeCharacter::OnTimeWarpReleased()
{
    if (bIsTimeWarping)
    {
        StopTimeWarp();
    }    
}

void APrototypeCharacter::StopTimeWarp()
{
    // Reset Time warp variables
    bWantsToTimeWarp = false;
    bIsTimeWarping = false;
    OverloadXPDrain = OverloadXPDrainDefault;

    // Make current energypickup speed back to normal
    TArray<AActor*> FoundEnergyPickupActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyPickup::StaticClass(), FoundEnergyPickupActors);

    for (auto Actor : FoundEnergyPickupActors)
    {
        AEnergyPickup* EnergyActor = Cast<AEnergyPickup>(Actor);
        // Normal its speed
        EnergyActor->SpeedLevel *= 2.0f;
    }

    UpdateTimeWarpSounds(bIsTimeWarping);
}

void APrototypeCharacter::OnUpgradePower1Pressed()
{
    UpgradePower(SpeedPowerLevel);

    UpdateSpeedPower();    
}

void APrototypeCharacter::UpdateSpeedPower()
{
    // Set new speed value
    RunSpeed = SpeedPowerDefault + ((SpeedPowerLevel - 1) * SpeedPowerIncrement);
    NormalAccel = AccelerationPowerDefault + ((SpeedPowerLevel - 1) * AccelerationPowerIncrement);
    GetCharacterMovement()->MaxAcceleration = NormalAccel;

    // Update speed
    GetCharacterMovement()->MaxWalkSpeed = (bIsRunning) ? RunSpeed : RunSpeed / 2;
    GetCharacterMovement()->MaxWalkSpeedCrouched = RunSpeed / 3;
}

void APrototypeCharacter::OnUpgradePower2Pressed()
{
    UpgradePower(JumpPowerLevel);

    UpdateJumpPower();
}

void APrototypeCharacter::UpdateJumpPower()
{
    // Set new jump velocity value
    GetCharacterMovement()->JumpZVelocity = JumpPowerDefault + ((JumpPowerLevel - 1) * JumpPowerIncrement);
}

void APrototypeCharacter::OnUpgradePower3Pressed()
{
    UpgradePower(StaminaPowerLevel);

    UpdateStaminaPower();
}

void APrototypeCharacter::UpdateStaminaPower()
{
    // Set new stamina drain value
    StaminaDrain = StaminaPowerDefault - ((StaminaPowerLevel - 1) * StaminaPowerIncrement);
}

void APrototypeCharacter::OnUpgradePower4Pressed()
{
    UpgradePower(AbsorbPowerLevel, false);

    UpdateAbsorbPower();
}

void APrototypeCharacter::UpdateAbsorbPower()
{
    // Set new absorb time and distance values
    AbsorbPowerTime = AbsorbPowerTimeDefault - ((AbsorbPowerLevel - 1) * AbsorbPowerTimeIncrement);
    AbsorbPowerDistance = AbsorbPowerDistanceDefault + ((AbsorbPowerLevel - 1) * AbsorbPowerDistanceIncrement);
}

void APrototypeCharacter::UpgradePower(float &PowerId, bool bIsNormal)
{
    int MaximumLevelForPower = (bIsNormal) ? MAX_LEVEL : MAX_LEVEL / 3;
    
    if (StatsCount > 0)
    {
        int OverloadedPowerCap = (bIsNormal) ? 3 : 1;
        MaximumLevelForPower = (bIsOverloaded) ? MaximumLevelForPower + OverloadedPowerCap : MaximumLevelForPower;

        if (PowerId < MaximumLevelForPower)
        {
            // Level up stat
            PowerId++;

            // Reduce stat available
            StatsCount--;

            // Play upgrade sound
            UGameplayStatics::PlaySoundAtLocation(this, PowerUpgradeSound, GetActorLocation());
        }        
    }
    
    if (bIsHackingPowerups)
    {
        // Level up stat or set it to 1 if over maximum x2
        PowerId = (PowerId < MaximumLevelForPower * 2) ? PowerId + 1 : PowerId = 1;
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

void APrototypeCharacter::OnContinueGamePressed()
{
    // Only allow to continue if in GameWon state
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (MyGameMode->GetCurrentState() == EPrototypePlayState::EGameWon)
    {
        // Enter the next game mode
        MyGameMode->SetNextState();
    }
}

void APrototypeCharacter::OnPauseGamePressed()
{
    // Toggle pause bool
    bIsGamePaused = !bIsGamePaused;

    // Get HUD
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    APrototypeHUD* MyGameHUD = Cast<APrototypeHUD>(PlayerController->GetHUD());
    MyGameHUD->bShowPause = bIsGamePaused;

    APlayerController* const MyPlayer = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
    MyPlayer->SetPause(bIsGamePaused);    
}

void APrototypeCharacter::MissingStamina()
{
    // Show missing stamina for 2 seconds
    MissingStaminaTimer = 2.0f;

    // Play missing stamina sound
    UGameplayStatics::PlaySoundAtLocation(this, MissingStaminaSound, GetActorLocation());
}

void APrototypeCharacter::Overload()
{
    // If can overload
    if (ExperiencePoints > 0.0f && OverloadCooldownTimer <= 0.0f && !bIsOverloaded)
    {
        bOverloadStarted = true;

        // Activate overload with a 4 second delay
        FTimerHandle UniqueHandle;
        FTimerDelegate OverloadDelegate = FTimerDelegate::CreateUObject(this, &APrototypeCharacter::StartOverload);
        GetWorldTimerManager().SetTimer(UniqueHandle, OverloadDelegate, 4.f, false);
    }    
    else if (ExperiencePoints <= 0.0f)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        APrototypeHUD* MyGameHUD = Cast<APrototypeHUD>(PlayerController->GetHUD());
        MyGameHUD->bShowOverloadMissingXP = true;
    }
    else if (OverloadCooldownTimer > 0.0f)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        APrototypeHUD* MyGameHUD = Cast<APrototypeHUD>(PlayerController->GetHUD());
        MyGameHUD->bShowOverloadMissingCD = true;
    }
}

void APrototypeCharacter::StartOverload()
{
    bIsOverloaded = true;
    bOverloadStarted = false;

    // Upgrade +3/+1
    SpeedPowerLevel += 3;
    JumpPowerLevel += 3;
    StaminaPowerLevel += 3;
    AbsorbPowerLevel += 1;

    // Update powers
    UpdateSpeedPower();
    UpdateJumpPower();
    UpdateStaminaPower();
    UpdateAbsorbPower();

    // Respawn faster
    RespawnTime = RespawnOverloadTime;

    // Make current energypickup slower x2
    TArray<AActor*> FoundEnergyPickupActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyPickup::StaticClass(), FoundEnergyPickupActors);

    for (auto Actor : FoundEnergyPickupActors)
    {
        AEnergyPickup* EnergyActor = Cast<AEnergyPickup>(Actor);
        // Slow its speed
        EnergyActor->SpeedLevel /= 2.0f;
    }

    // Make spawn volume spawn slower x2
    TArray<AActor*> FoundSpawnVolumeActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);

    // Get the present spawn volume
    for (auto Actor : FoundSpawnVolumeActors)
    {
        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
        SpawnVolumeActor->bIsCharacterOverloaded = true;
    }
}

void APrototypeCharacter::EndOverload()
{
    bIsOverloaded = false;
    bOverloadEnded = true;
    OverloadCooldownTimer = OverloadCooldownDuration;

    // Reset Time warp
    StopTimeWarp();

    // Deupgrade -3/-1
    SpeedPowerLevel -= 3;
    JumpPowerLevel -= 3;
    StaminaPowerLevel -= 3;
    AbsorbPowerLevel -= 1;

    // Update powers
    UpdateSpeedPower();
    UpdateJumpPower();
    UpdateStaminaPower();
    UpdateAbsorbPower();

    // Respawn faster
    RespawnTime = RespawnNormalTime;

    // Make current energypickup normal
    TArray<AActor*> FoundEnergyPickupActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyPickup::StaticClass(), FoundEnergyPickupActors);

    for (auto Actor : FoundEnergyPickupActors)
    {
        AEnergyPickup* EnergyActor = Cast<AEnergyPickup>(Actor);
        // Slow its speed
        EnergyActor->SpeedLevel *= 2.0f;
    }

    // Make spawn volume spawn slower
    TArray<AActor*> FoundSpawnVolumeActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundSpawnVolumeActors);

    // Get the present spawn volume
    for (auto Actor : FoundSpawnVolumeActors)
    {
        ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
        SpawnVolumeActor->bIsCharacterOverloaded = false;
    }
}

void APrototypeCharacter::OnHack1Pressed()
{
    if (bCanHack)
    {
        Hack1Ctr--;
        if (Hack1Ctr < 0)
        {
            Hack1Ctr = 0;
        }

        if (Hack2Ctr == 0)
        {
            bCanHack = false;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("System locked"));
        }        
    }
    else if (!bCanHack)
    {
        Hack1Ctr++;
        if (Hack1Ctr > 1)
        {
            Hack1Ctr = 1;
        }

        if (Hack2Ctr == 2)
        {
            bCanHack = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("System unlocked"));
        }
    }
}

void APrototypeCharacter::OnHack2Pressed()
{
    if (bCanHack)
    {
        Hack2Ctr--;
        if (Hack2Ctr < 0)
        {
            Hack2Ctr = 0;
        }

        if (Hack1Ctr == 0 && Hack2Ctr == 0)
        {
            bCanHack = false;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("System locked"));
        }
    }
    else if (!bCanHack)
    {
        Hack2Ctr++;
        if (Hack2Ctr > 2)
        {
            Hack1Ctr = 2;
        }

        if (Hack2Ctr == 2 && Hack1Ctr == 1)
        {
            bCanHack = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("System unlocked"));
        }
    }
}


void APrototypeCharacter::System0731()
{
    if (!bCanHack){ return; }

    bIsHackingPowerups = !bIsHackingPowerups;

    if (bIsHackingPowerups)
    {
        // Upgrade all to double max
        SpeedPowerLevel = MAX_LEVEL * 2 - 1;
        JumpPowerLevel = MAX_LEVEL * 2 - 1;
        StaminaPowerLevel = MAX_LEVEL * 2 - 1;
        AbsorbPowerLevel = (MAX_LEVEL / 3.0f) * 2 - 1;

        OnUpgradePower1Pressed();
        OnUpgradePower2Pressed();
        OnUpgradePower3Pressed();
        OnUpgradePower4Pressed();
    }    
}

void APrototypeCharacter::System0732()
{
    if (!bCanHack){ return; }

    ExperiencePoints = MAX_EXPERIENCE;
    bIsHackingExperience = !bIsHackingExperience;
}

void APrototypeCharacter::System0733()
{
    if (!bCanHack){ return; }

    // Get game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->SetExplosionCount(-1000.0f);
}

void APrototypeCharacter::System0734()
{
    if (!bCanHack){ return; }

    // Get game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy(5);    
}

void APrototypeCharacter::System0735()
{
    if (!bCanHack){ return; }

    // Get game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy(15);
}

void APrototypeCharacter::System0736()
{
    if (!bCanHack){ return; }

    // Get game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy(30);
}

void APrototypeCharacter::System0737()
{
    if (!bCanHack){ return; }

    // Get game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    MyGameMode->IncrementEnergy(50);
}

//////////////////////////////////////////////////////////////////////////
// Base Movement Override
void APrototypeCharacter::OnLanded(const FHitResult& Hit)
{
    // In order to slide, must press crouch, not be walking (1.9f), and grounded
    if (GetCharacterMovement()->bWantsToCrouch && GetCharacterMovement()->Velocity.Size2D() > (RunSpeed / 1.9f))
    {
        VerifySlide();
    }
    
    // If player is currently holding shift
    if (bIsRunning)
    {
        // Play run sound
        UpdateRunSounds(true);
    }

    // Change the fall speed to normal
    GetPawnPhysicsVolume()->TerminalVelocity = FallSpeed;

    // Calculate land sound volume multiplier
    float LandVolumeMultiplier;
    if (DiveSpeed > 0.0f)
    {
        LandVolumeMultiplier = DiveSpeed / FallSpeed;

        // Don't go too high
        if (LandVolumeMultiplier > 3.0f)
        {
            LandVolumeMultiplier = 3.0f;
        }

        // Reset dive speed
        DiveSpeed = 0.0f;
    }
    else
    {
        LandVolumeMultiplier = 1.0f;
    }
    
    // Parent call
    Super::OnLanded(Hit);

    // Play land sound
    UGameplayStatics::PlaySoundAtLocation(this, LandSound, GetActorLocation(), LandVolumeMultiplier);    

    if (bIsOverloaded)
    {
        // Play overload land sound
        UGameplayStatics::PlaySoundAtLocation(this, OverloadLandSound, GetActorLocation(), LandVolumeMultiplier);
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
        else if (RunAC && !RunAC->IsPlaying())
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
        else if (WindAC && !WindAC->IsPlaying())
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

void APrototypeCharacter::UpdateSlideSounds(bool bNewIsSliding)
{
    if (bNewIsSliding)
    {
        // If the wall ride audio component isn't created yet
        if (!SlideAC && SlideSound)
        {
            // Initialize the run audio component
            SlideAC = UGameplayStatics::PlaySoundAttached(SlideSound, GetRootComponent());
            if (SlideAC)
            {
                SlideAC->bAutoDestroy = false;
            }
        }
        else if (SlideAC && !SlideAC->IsPlaying())
        {
            SlideAC->Play();
        }
    }
    else
    {
        if (SlideAC)
        {
            SlideAC->Stop();
        }
    }
}

void APrototypeCharacter::UpdateTimeWarpSounds(bool bNewIsTimeWarping)
{
    if (bNewIsTimeWarping)
    {
        // If the time warp audio component isn't created yet
        if (!TimeWarpAC && TimeWarpSound)
        {
            // Initialize the run audio component
            TimeWarpAC = UGameplayStatics::PlaySoundAttached(TimeWarpSound, GetRootComponent());
            if (TimeWarpAC)
            {
                TimeWarpAC->bAutoDestroy = false;
            }
        }
        else if (TimeWarpAC && !TimeWarpAC->IsPlaying())
        {
            TimeWarpAC->Play();
        }
    }
    else
    {
        if (TimeWarpAC)
        {
            TimeWarpAC->Stop();
        }
    }
}

void APrototypeCharacter::UpdateOverloadDashSounds(bool bNewIsOverloadDashing)
{
    if (bNewIsOverloadDashing)
    {
        // If the time warp audio component isn't created yet
        if (!OverloadDashAC && OverloadDashSound)
        {
            // Initialize the run audio component
            OverloadDashAC = UGameplayStatics::PlaySoundAttached(OverloadDashSound, GetRootComponent());
            if (OverloadDashAC)
            {
                OverloadDashAC->bAutoDestroy = false;
            }
        }
        else if (OverloadDashAC && !OverloadDashAC->IsPlaying())
        {
            OverloadDashAC->Play();
        }
    }
    else
    {
        if (OverloadDashAC)
        {
            OverloadDashAC->Stop();
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// Powerups
void APrototypeCharacter::LevelUp(bool bLeveledUpWithXP)
{
    // Increment stat
    StatsCount++;

    // Full stamina
    StaminaCurrent = StaminaMax;
    
    if (bLeveledUpWithXP)
    {
        // Play level up sound
        UGameplayStatics::PlaySoundAtLocation(this, LevelUpXPSound, GetActorLocation());
    }
    else
    {
        // Play level up sound
        UGameplayStatics::PlaySoundAtLocation(this, LevelUpPickupSound, GetActorLocation());
    }    
}


//////////////////////////////////////////////////////////////////////////
// Spawning
void APrototypeCharacter::Die()
{
    // Set death state to true
    bIsDead = true;

    // Remove any velocity
    GetCharacterMovement()->Velocity = FVector(0,0,0);

    // Change the fall speed
    GetPawnPhysicsVolume()->TerminalVelocity = FallSpeed / 50.f;

    // Disable movement
    bMovementBlocked = true;

    // Play death sound
    UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    
    // Finish the respawn with a 2 second delay
    FTimerHandle UniqueHandle;
    FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &APrototypeCharacter::Respawn);
    GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, RespawnTime, false);
}

void APrototypeCharacter::Respawn()
{
    // Enable movement
    bMovementBlocked = false;

    // Change the fall speed back to normal
    GetPawnPhysicsVolume()->TerminalVelocity = FallSpeed;

    // Position the player
    SetActorLocation(RespawnPosition + FVector(0, 0, 1000));

    // Play respawn sound
    UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());

    // Set death state to false
    bIsDead = false;
}

