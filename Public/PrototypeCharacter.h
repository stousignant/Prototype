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
    
    /** Dash update */
    void UpdateDash();

    /** Stamina update */
    void UpdateStamina();

    /** Wall ride update */
    void UpdateWallRide(float DeltaSeconds);

    /** Respawn point saving update */
    void UpdateRespawnPoint();

    /** Scanning update */
    void UpdateScan();


    //////////////////////////////////////////////////////////////////////////
    // Camera & aim
   	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

    /** Camera position relative to the character's position */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    FVector CameraRelativePosition;

    /** Distance of the trace for scanning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanDistance;

    /** Scan amount in order to end */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanMaximum;

    /** Scan starting offset for HUD */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanOffset1;

    /** Scan ending offset for HUD */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scan)
    float ScanOffset2;

    /** If the character is currently pressing the scan input */
    bool bWantsToScan;

    /** If the character is currently scanning an energy sphere */
    bool bIsScanning;

    /** Scanning progress */
    float ScanProgress;

    /** Amount of energy scanned */
    float EnergyCount;



    //////////////////////////////////////////////////////////////////////////
    // Input
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
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

    /** Current jumping state */
    UPROPERTY(Transient)
    bool bWantsToJump;
    
    /** If the character's movements are blocked */
    bool bMovementBlocked;

    /** If the character going fast */
    bool bIsFast;

    /** Amount of the current stamina */
    float StaminaCurrent;

    /** Current wall riding state */
    UPROPERTY(Transient)
    bool bIsWallRiding;

    /** If the character is in the death state */
    UPROPERTY(Transient)
    bool bIsDead;

    /** Amount of the max stamina */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float StaminaMax;

    /** Amount of stamina drained by dashing */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float StaminaDrain;
    
    /** Amount of stamina replenished when not dashing */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float StaminaReplenish;

    /** Normal fall speed */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float FallSpeed;

    /** Dive speed */
    UPROPERTY(EditDefaultsOnly, Category = Gameplay)
    float DiveSpeed;
    
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

    /** Player pressed scan action */
    void OnScanPressed();

    /** Player released scan action */
    void OnScanReleased();

    /** Player pressed upgrade power 1 action */
    void OnUpgradePower1Pressed();

    /** Player pressed upgrade power 2 action */
    void OnUpgradePower2Pressed();

    /** Player pressed upgrade power 3 action */
    void OnUpgradePower3Pressed();

    
    
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

    /** Sound to play when we land */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* LandSound;

    /** Sound to play when we travel at huge speed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* WindSound;

    /** Used to manipulate with wind loop sound */
    UPROPERTY()
    UAudioComponent* WindAC;
    
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
    
    /** Sound to play when we upgrade a power */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    class USoundBase* PowerUpgradeSound;

    /** Handles sounds for running */
    void UpdateRunSounds(bool bNewRunning);

    /** Handles sounds for the wind */
    void UpdateWindSounds(bool bNewIsFast);

    /** Handles sounds for the wall ride */
    void UpdateWallRideSounds(bool bNewIsWallRiding);

    /** Handles sounds for the wind */
    void UpdateSounds(UAudioComponent* SoundAC, bool bPlaySound);


    //////////////////////////////////////////////////////////////////////////
    // Powerups
    /** Collection Volume in order to interact with powerups */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Power)
    USphereComponent* CollectionSphere;

    /** Stats unspent of our character */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Power)
    float StatsCount;

    /** Speed power level of our character */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Power)
    float SpeedPowerLevel;

    /** Jump power level of our character */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Power)
    float JumpPowerLevel;

    /** Stamina power level of our character */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Power)
    float StaminaPowerLevel;


    //////////////////////////////////////////////////////////////////////////
    // Spawning
    /** Vector storing the last position for respawn */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    FVector RespawnPosition;

    /** Delay used between saves of respawn positions */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveDelay;

    /** Timer used to save a new respawn point at each RespawnSaveDelay */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawning)
    float RespawnSaveTimer;

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

    /** Function used by the OnUpgradePowerXPressed */
    void UpgradePower(float &PowerId);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

