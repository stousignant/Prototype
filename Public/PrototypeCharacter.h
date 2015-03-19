// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "PrototypeCharacter.generated.h"

UCLASS(config=Game)
class APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    //////////////////////////////////////////////////////////////////////////
    // Constructor
    APrototypeCharacter(const FObjectInitializer& ObjectInitializer);


    //////////////////////////////////////////////////////////////////////////
    // Update
    /** Tick */ 
    virtual void Tick(float DeltaSeconds) override;
    
    /** Run update */
    void UpdateRun(float DeltaSeconds);

    /** Dash update */
    void UpdateDash(float DeltaSeconds);

    /** Stamina update */
    void UpdateStamina(float DeltaSeconds);

    /** Wall ride update */
    void UpdateWallRide(float DeltaSeconds);

    /** Respawn point saving update */
    void UpdateRespawnPoint(float DeltaSeconds);

    /** Scanning update */
    void UpdateScan(float DeltaSeconds);

    /** Sliding update */
    void UpdateSlide(float DeltaSeconds);

    /** Powerup detection update */
    void UpdatePowerupDetection(float DeltaSeconds);

    /** Manage the wind speed sound */
    void UpdateSound(float DeltaSeconds);

    /** Manage the overload */
    void UpdateOverload(float DeltaSeconds);


    //////////////////////////////////////////////////////////////////////////
    // Camera & aim
   	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

    /** Camera position relative to the character's position */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    FVector CameraRelativePosition;
    
    /** Scan starting offset for HUD */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanOffset1;

    /** Scan ending offset for HUD */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanOffset2;

    /** If the character is currently pressing the scan input */
    bool bWantsToScan;

    /** Distance of the trace for scanning */
    float ScanDistance;

    /** If the character is currently scanning an energy sphere */
    bool bIsScanning;

    /** Scanning progress */
    float ScanProgress;

    /** Amount of energy scanned */
    float EnergyCount;



    //////////////////////////////////////////////////////////////////////////
    // Input
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
    
    /** Frame rate independent turn */
    void TurnAtRate(float Rate);

    /** Frame rate independent lookup */
    void LookUpAtRate(float Rate);

    /** Walk speed for our character */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float WalkSpeed;

    /** Modifier for the running speed */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float RunSpeed;

    /** Modifier for the normal acceleration */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float NormalAccel;

    /** Modifier for the dashing speed */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float DashSpeed;

    /** Modifier for the dashing acceleration */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float DashAccel;

    /** Modifier for the wind control */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float WindControlModifier;

    /** Current running state */
    UPROPERTY(Transient)
    bool bIsRunning;
        
    /** Current dashing state */
    UPROPERTY(Transient)
    bool bIsDashing;

    /** Current sliding state */
    UPROPERTY(Transient)
    bool bIsSliding;

    /** Current jumping state */
    UPROPERTY(Transient)
    bool bWantsToJump;

    /** Current crouch state */
    UPROPERTY(Transient)
    bool bWantsToCrouch;

    /** If player started overload action */
    UPROPERTY(Transient)
    bool bOverloadStarted;

    /** If overload action ended*/
    UPROPERTY(Transient)
    bool bOverloadEnded;

    /** If character is in overload state */
    bool bIsOverloaded;

    /** Cooldown timer for the overload state */
    float OverloadCooldownTimer;

    /** Value for the cooldown duration */
    float OverloadCooldownDuration;

    /** Value for the xp drain when overloaded */
    float OverloadXPDrain;

    /** Default value for the xp drain when overloaded */
    float OverloadXPDrainDefault;

    /** Time warp value for the xp drain when overloaded */
    float OverloadXPDrainTimeWarp;

    /** */
    void StartOverload();

    /** */
    void EndOverload();
    
    /** If the character's movements are blocked */
    bool bMovementBlocked;

    /** If the character going fast */
    bool bIsFast;

    /** If the character is missing stamina for an action */
    bool bIsMissingStamina;

    /** Missing stamina timer */
    float MissingStaminaTimer;
    
    /** Amount of the current stamina */
    float StaminaCurrent;

    /** Base Stamina drained from sliding */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float SlideStaminaConsumption;

    /** Base Stamina drained from dive */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float DiveStaminaConsumption;

    /** Base Stamina drained from dash jump */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float DashJumpStaminaConsumption;

    /** Current wall riding state */
    UPROPERTY(Transient)
    bool bIsWallRiding;

    /** If the character is in the death state */
    UPROPERTY(Transient)
    bool bIsDead;

    /** Amount of the max stamina */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float StaminaMax;

    /** Amount of stamina drained by dashing */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float StaminaDrain;
    
    /** Amount of stamina replenished when not dashing */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float StaminaReplenish;

    /** Normal fall speed */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float FallSpeed;
    
	/** Handles moving forward/backward */
	void OnMoveForward(float Val);

	/** Handles stafing movement, left and right */
	void OnMoveRight(float Val);

    /** player pressed jump action */
    void OnJumpPressed();

    /** player released jump action */
    void OnJumpReleased();
    
    /** Player pressed run action */
    void OnRunPressed();

    /** Player released run action */
    void OnRunReleased();

    /** Change running state */
    void SetRunning(bool bNewRunning);
    
    /** Player pressed dash action */
    void OnDashPressed();
    
    /** Player released dash action */
    void OnDashReleased();

    /** Player pressed dive action */
    void OnDivePressed();

    /** Player pressed crouch action */
    void OnCrouchPressed();

    /** Player released crouch action */
    void OnCrouchReleased();

    /** Verify if slide and can */
    void VerifySlide();

    /** Change sliding state */
    void SetSliding(bool bNewSliding);

    /** Player pressed scan action */
    void OnScanPressed();

    /** Player released scan action */
    void OnScanReleased();

    /** Player pressed upgrade power 1 action */
    void OnUpgradePower1Pressed();

    /** */
    void UpdateSpeedPower();

    /** Player pressed upgrade power 2 action */
    void OnUpgradePower2Pressed();

    /** */
    void UpdateJumpPower();

    /** Player pressed upgrade power 3 action */
    void OnUpgradePower3Pressed();

    /** */
    void UpdateStaminaPower();

    /** Player pressed upgrade power 4 action */
    void OnUpgradePower4Pressed();

    /** */
    void UpdateAbsorbPower();

    /** Player pressed the exit game key */
    void OnExitGamePressed();

    /** Player pressed the restart game key */
    void OnRestartGamePressed();

    /** Player pressed the continue game key */
    void OnContinueGamePressed();

    /** Player pressed the pause game key */
    void OnPauseGamePressed();

    /** Player pressed the hack1 key */
    void OnHack1Pressed();

    /** Player pressed the hack2 key */
    void OnHack2Pressed();

    /** Player pressed the time warp action */
    void OnTimeWarpPressed();

    /** Player released the time warp action */
    void OnTimeWarpReleased();

    /** */
    void StopTimeWarp();

    /** If the player is currently pressing the time warp input */
    bool bWantsToTimeWarp;

    /** */
    bool bIsTimeWarping;
    
    //////////////////////////////////////////////////////////////////////////
    // Base Movement Override
    /** When the character lands */
    virtual void OnLanded(const FHitResult& Hit) override;

    /** When the character is falling */
    virtual void Falling() override;

    /***/
    virtual bool CanJumpInternal_Implementation() const override;
    

    //////////////////////////////////////////////////////////////////////////
    // Sound
    /** Sound to play while we run */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* RunSound;
    
    /** Used to manipulate with run sound */
    UPROPERTY()
    UAudioComponent* RunAC;

    /** Sound to play when we jump */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* JumpSound;

    /** Sound to play when we jump in overload */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* OverloadJumpSound;

    /** Sound to play when we land */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* LandSound;

    /** Sound to play when we land in overload */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* OverloadLandSound;

    /** Sound to play when we travel at huge speed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* WindSound;

    /** Used to manipulate with wind loop sound */
    UPROPERTY()
    UAudioComponent* WindAC;

    /** Sound to play when we dash in overload */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* OverloadDashSound;

    /** Used to manipulate with dash loop sound */
    UPROPERTY()
    UAudioComponent* OverloadDashAC;
    
    /** Sound to play when the scan is completed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* ScanSound;

    /** Sound to play when dying */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* DeathSound;

    /** Sound to play when respawning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* RespawnSound;
    
    /** Sound to play when we wall ride */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* WallRideSound;

    /** Used to manipulate with wall ride loop sound */
    UPROPERTY()
    UAudioComponent* WallRideAC;

    /** Sound to play when we slide */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* SlideSound;

    /** Used to manipulate with slide loop sound */
    UPROPERTY()
    UAudioComponent* SlideAC;
    
    /** Sound to play when we upgrade a power */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* PowerUpgradeSound;

    /** Sound to play when missing stamina */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* MissingStaminaSound;

    /** Sound to play when leveling up with xp */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* LevelUpXPSound;

    /** Sound to play when leveling up with pickup */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* LevelUpPickupSound;

    /** Sound to play when time warping */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* TimeWarpSound;

    /** Used to manipulate with time warp loop sound */
    UPROPERTY()
    UAudioComponent* TimeWarpAC;

    /** Handles sounds for running */
    void UpdateRunSounds(bool bNewRunning);

    /** Handles sounds for the wind */
    void UpdateWindSounds(bool bNewIsFast);

    /** Handles sounds for the wall ride */
    void UpdateWallRideSounds(bool bNewIsWallRiding);

    /** Handles sounds for the slide */
    void UpdateSlideSounds(bool bNewIsSliding);

    /** Handles sounds for the wind */
    void UpdateSounds(UAudioComponent* SoundAC, bool bPlaySound);

    /** Handles sounds for the time warp */
    void UpdateTimeWarpSounds(bool bNewIsTimeWarping);

    /** Handles sounds for the overload dash */
    void UpdateOverloadDashSounds(bool bNewIsOverloadDashing);


    //////////////////////////////////////////////////////////////////////////
    // Powerups
    /** Collection Volume in order to interact with powerups */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Power)
    USphereComponent* CollectionSphere;

    /** Timer delay for detection */
    float PowerupDetectionTimer;

    /** Number of powerups close range */
    float PowerupDetectedCount;

    /** Stats unspent of our character */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float StatsCount;

    /** Speed power level of our character */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float SpeedPowerLevel;

    /** Speed power increment per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float SpeedPowerIncrement;

    /** Speed power for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float SpeedPowerDefault;

    /** Acceleration power increment per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AccelerationPowerIncrement;

    /** Acceleration power for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AccelerationPowerDefault;

    /** Jump power level of our character */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float JumpPowerLevel;

    /** Jump power increment per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float JumpPowerIncrement;

    /** Jump power for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float JumpPowerDefault;

    /** Stamina power level of our character */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float StaminaPowerLevel;

    /** Stamina power per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float StaminaPowerIncrement;

    /** Stamina power for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float StaminaPowerDefault;

    /** Absorb power level of our character */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerLevel;

    /** Absorb time per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerTimeIncrement;

    /** Absorb time */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerTime;

    /** Absorb time for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerTimeDefault;

    /** Absorb distance per level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerDistanceIncrement;

    /** Absorb distance */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerDistance;
    
    /** Absorb distance for level 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float AbsorbPowerDistanceDefault;
        
    /** Experience */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Power)
    float ExperiencePoints;

    /** */
    float ExperiencePointsNewlyGained;

    /** */
    void LevelUp(bool bLeveledUpWithXP);    


    //////////////////////////////////////////////////////////////////////////
    // Spawning
    /** Vector storing the last position for respawn */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    FVector RespawnPosition;

    /** Delay used between saves of respawn positions */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveNormalDelay;

    /** Delay used between saves of respawn positions while being overloaded */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveOverloadDelay;

    /** Timer used to save a new respawn point at each RespawnSaveDelay */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveTimer;

    /** Timer used to ensure the new espawn point is safe */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveFailnetTimer;

    /** Current duration to respawn */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnTime;

    /** Duration to respawn when not overloaded */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnNormalTime;

    /** Duration to respawn while being overloaded */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnOverloadTime;

    /** Respawns the character */
    UFUNCTION(BlueprintCallable, Category = Spawning)
    void Die();

    /** Finalize the respawn event */
    UFUNCTION(BlueprintCallable, Category = Spawning)
    void Respawn();

    //////////////////////////////////////////////////////////////////////////
    // Misc
    /** Material used in order to know if the mesh is walljumpable */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    UMaterial* WallJumpMaterial;  

    /** Wallride distance */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    float WallRideDistance;

    /** Function used by the OnUpgradePowerXPressed */
    void UpgradePower(float &PowerId, bool bIsNormal = true);

    /** Show that character is lacking stamina for action */
    void MissingStamina();

    /** */
    float DiveSpeed;

    /** Overload command */
    UFUNCTION(Exec)
    void Overload();

    /** */
    bool bCanHack;

    /** */
    float Hack1Ctr;
    float Hack2Ctr;

    /** Is cheat for powerups on */
    bool bIsHackingPowerups;

    /** Hack powerups command */
    UFUNCTION(Exec)
    void System0731();

    /** If game is paused */
    bool bIsGamePaused;

    /** Is cheat for experience on */
    bool bIsHackingExperience;

    /** Hack experience command */
    UFUNCTION(Exec)
    void System0732();

    /** Is cheat for planet lifes on */
    bool bIsHackingLife;

    /** Hack life command */
    UFUNCTION(Exec)
    void System0733();

    /** Hack normal mode command */
    UFUNCTION(Exec)
    void System0734();

    /** Hack hard mode command */
    UFUNCTION(Exec)
    void System0735();

    /** Hack very hard mode command */
    UFUNCTION(Exec)
    void System0736();

    /** Hack ultimate mode command */
    UFUNCTION(Exec)
    void System0737();
        

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

