// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "PrototypeGameMode.h"
#include "PrototypeHUD.generated.h"

UCLASS()
class APrototypeHUD : public AHUD
{
	GENERATED_BODY()

public:
    // Constructor
	APrototypeHUD(const FObjectInitializer& ObjectInitializer);

    /** Variable for storing the font */
    UPROPERTY()
    UFont* HUDFont;

    /** Game mode */
    APrototypeGameMode* MyGameMode;

    /** Screen dimensions */
    FVector2D ScreenDimensions;
    
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

    /** Draw the player information such as speed, stamina, etc */
    void DrawPlayerInfo();

    /** Draw the player powers */
    void DrawPlayerPowers();
    
    /** Draw the game information such as objectives, etc */
    void DrawGameInfo();
    
    /** Draw the scan UI */
    void DrawScan();

    /** Draw the game over state */
    void DrawGameOver();

    /** Draw the game won state */
    void DrawGameWon();

    /** Scanning progress value */
    float ScanProgress;

    /** */
    bool bShowXPGain;

    /** */
    bool bShowLevelUp;

    /** */
    float XPGainTimer;

    /** */
    float LevelUpTimer;

    /** */
    bool bShowOverloadMissingXP;

    /** */
    bool bShowOverloadMissingCD;

    /** */
    float OverloadMissingTimer;

    /** */
    bool bShowPause;


};

