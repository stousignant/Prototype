// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype.h"
#include "PrototypeHUD.h"
#include "PrototypeCharacter.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"

const float LEFTSIDE_X = 3.4f;
const float RIGHTSIDE_X = 1.42f;
const float UPSIDE_Y = 3.0f;
const float DOWNSIDE_Y = 1.4f;


APrototypeHUD::APrototypeHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Use the RobotoDistanceField font from the engine
    static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = HUDFontOb.Object;
}

void APrototypeHUD::DrawHUD()
{
    // Parent call
	Super::DrawHUD();

    // Get the screen dimensions
    ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

    // If the game is over 
    MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(this));
    if (MyGameMode->GetCurrentState() == EPrototypePlayState::EGameOver)
    {
        DrawGameOver();
    }
    // If the game is won
    else if (MyGameMode->GetCurrentState() == EPrototypePlayState::EGameWon)
    {
        DrawGameWon();
    }
    // Else render the game UI
    else
    {
        DrawPlayerInfo();

        DrawPlayerPowers();

        DrawGameInfo();
        
        DrawScan();
    }       
}

void APrototypeHUD::DrawPlayerInfo()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Print speed
    FString SpeedString = FString::Printf(TEXT("Speed  %0.1f km/h"), MyCharacter->GetVelocity().Size() / 100);
    FVector2D SpeedStringSize;
    GetTextSize(SpeedString, SpeedStringSize.X, SpeedStringSize.Y, HUDFont);
    DrawText(SpeedString, FColor::Yellow, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / UPSIDE_Y), HUDFont);

    // Print stamina
    FString StaminaString = FString::Printf(TEXT("Stamina %0.f"), MyCharacter->StaminaCurrent);
    FVector2D StaminaStringSize;
    GetTextSize(StaminaString, StaminaStringSize.X, StaminaStringSize.Y, HUDFont);
    DrawText(StaminaString, FColor::Green, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / UPSIDE_Y) + SpeedStringSize.Y, HUDFont);

    // Print missing stamina
    if (MyCharacter->bIsMissingStamina)
    {
        FString MissingStaminaString = FString::Printf(TEXT("Missing Stamina"));
        FVector2D MissingStaminaStringSize;
        GetTextSize(MissingStaminaString, MissingStaminaStringSize.X, MissingStaminaStringSize.Y, HUDFont);
        DrawText(MissingStaminaString, FColor::Red, (ScreenDimensions.X - MissingStaminaStringSize.X) / 2.0f, (ScreenDimensions.Y - MissingStaminaStringSize.Y) / 2.0f, HUDFont);
    }

    // Print respawn
    if (MyCharacter->bIsDead)
    {
        FString RespawnString = FString::Printf(TEXT("Respawning"));
        FVector2D RespawnStringSize;
        GetTextSize(RespawnString, RespawnStringSize.X, RespawnStringSize.Y, HUDFont);
        DrawText(RespawnString, FColor::Cyan, (ScreenDimensions.X - RespawnStringSize.X) / 2.0f, (ScreenDimensions.Y - RespawnStringSize.Y) / 2.45f, HUDFont);
    }

    // Print powerup detection
    if (MyCharacter->PowerupDetectedCount > 0)
    {
        FString PowerupDetectedCountString = FString::Printf(TEXT("%0.f Powerup(s) detected"), MyCharacter->PowerupDetectedCount);
        FVector2D PowerupDetectedCountStringSize;
        GetTextSize(PowerupDetectedCountString, PowerupDetectedCountStringSize.X, PowerupDetectedCountStringSize.Y, HUDFont);
        DrawText(PowerupDetectedCountString, FColor::Blue, (ScreenDimensions.X / RIGHTSIDE_X) - PowerupDetectedCountStringSize.X, (ScreenDimensions.Y / DOWNSIDE_Y) - PowerupDetectedCountStringSize.Y, HUDFont);
    }

    // Print if crouch
    if (MyCharacter->bIsCrouched)
    {
        FString CrouchString = FString::Printf(TEXT("Crouched"));
        FVector2D CrouchStringSize;
        GetTextSize(CrouchString, CrouchStringSize.X, CrouchStringSize.Y, HUDFont);
        DrawText(CrouchString, FColor::Red, (ScreenDimensions.X / RIGHTSIDE_X) - CrouchStringSize.X, (ScreenDimensions.Y / DOWNSIDE_Y) - CrouchStringSize.Y * 2, HUDFont);
    }

    
}

void APrototypeHUD::DrawPlayerPowers()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Color of the text based on the unspent stats
    FColor ColorToDisplay = (MyCharacter->StatsCount > 0) ? FColor::Blue : FColor::White;

    // Print xp bar
    FColor FXPColor = (MyCharacter->ExperiencePoints == 100.0f) ? FColor::Blue : FColor::Cyan;
    FString ExperiencePointsString = FString::Printf(TEXT("XP"));
    FVector2D ExperiencePointsStringSize;
    GetTextSize(ExperiencePointsString, ExperiencePointsStringSize.X, ExperiencePointsStringSize.Y, HUDFont);
    DrawText(ExperiencePointsString, FXPColor, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / DOWNSIDE_Y) + ExperiencePointsStringSize.Y, HUDFont);
    DrawRect(FXPColor, (ScreenDimensions.X / LEFTSIDE_X) + ExperiencePointsStringSize.X + 5, (ScreenDimensions.Y / DOWNSIDE_Y) + ExperiencePointsStringSize.Y + 7, (ScreenDimensions.X / 263) * MyCharacter->ExperiencePoints, 10);

    // Print stamina power
    FString StaminaLevelString = FString::Printf(TEXT("Stamina Lv.%0.f"), MyCharacter->StaminaPowerLevel);
    FVector2D StaminaLevelStringSize;
    GetTextSize(StaminaLevelString, StaminaLevelStringSize.X, StaminaLevelStringSize.Y, HUDFont);
    DrawText(StaminaLevelString, ColorToDisplay, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / DOWNSIDE_Y), HUDFont);

    // Print jump power
    FString JumpLevelString = FString::Printf(TEXT("Jump Lv.%0.f"), MyCharacter->JumpPowerLevel);
    FVector2D JumpLevelStringSize;
    GetTextSize(JumpLevelString, JumpLevelStringSize.X, JumpLevelStringSize.Y, HUDFont);
    DrawText(JumpLevelString, ColorToDisplay, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / DOWNSIDE_Y) - StaminaLevelStringSize.Y, HUDFont);

    // Print speed power
    FString SpeedLevelString = FString::Printf(TEXT("Speed Lv.%0.f"), MyCharacter->SpeedPowerLevel);
    FVector2D SpeedLevelStringSize;
    GetTextSize(SpeedLevelString, SpeedLevelStringSize.X, SpeedLevelStringSize.Y, HUDFont);
    DrawText(SpeedLevelString, ColorToDisplay, (ScreenDimensions.X / LEFTSIDE_X), (ScreenDimensions.Y / DOWNSIDE_Y) - StaminaLevelStringSize.Y - JumpLevelStringSize.Y, HUDFont);

    // Print hack
    if (MyCharacter->bIsHackingPowerups)
    {
        FString HackString = FString::Printf(TEXT("SYSTEM HACKED"));
        FVector2D HackStringSize;
        GetTextSize(HackString, HackStringSize.X, HackStringSize.Y, HUDFont);
        DrawText(HackString, FColor::Yellow, (ScreenDimensions.X / RIGHTSIDE_X) - HackStringSize.X, (ScreenDimensions.Y / DOWNSIDE_Y), HUDFont);
    }
    // Print stats count
    else if (MyCharacter->StatsCount > 0)
    {
        FString StatsCountString = FString::Printf(TEXT("%0.f new stat(s)"), MyCharacter->StatsCount);
        FVector2D StatsCountStringSize;
        GetTextSize(StatsCountString, StatsCountStringSize.X, StatsCountStringSize.Y, HUDFont);
        DrawText(StatsCountString, ColorToDisplay, (ScreenDimensions.X / RIGHTSIDE_X) - StatsCountStringSize.X, (ScreenDimensions.Y / DOWNSIDE_Y), HUDFont);
    }    
}

void APrototypeHUD::DrawGameInfo()
{
    // Get the game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    
    // Print the energy count
    FString EnergyCountString = FString::Printf(TEXT("%0.0f/%0.0f Energy absorbed"), MyGameMode->GetEnergyCount(), MyGameMode->EnergyMax);
    FVector2D EnergyCountStringSize;
    GetTextSize(EnergyCountString, EnergyCountStringSize.X, EnergyCountStringSize.Y, HUDFont);
    DrawText(EnergyCountString, FColor::Cyan, (ScreenDimensions.X / RIGHTSIDE_X) - EnergyCountStringSize.X, (ScreenDimensions.Y / UPSIDE_Y), HUDFont);

    // Print the explosion count
    FString ExplosionCountString = FString::Printf(TEXT("%0.0f Planet lifes"), MyGameMode->ExplosionMax - MyGameMode->GetExplosionCount());
    FVector2D ExplosionCountStringSize;
    GetTextSize(ExplosionCountString, ExplosionCountStringSize.X, ExplosionCountStringSize.Y, HUDFont);
    DrawText(ExplosionCountString, FColor::Red, (ScreenDimensions.X / RIGHTSIDE_X) - ExplosionCountStringSize.X, (ScreenDimensions.Y / UPSIDE_Y) + EnergyCountStringSize.Y, HUDFont);

    // Print the time
    FString GameTimeString = FString::Printf(TEXT("%02.0f:%02.0f"), floor(MyGameMode->GameTime / 60.0f), fmod(MyGameMode->GameTime, 60.0f));
    FVector2D GameTimeStringSize;
    GetTextSize(GameTimeString, GameTimeStringSize.X, GameTimeStringSize.Y, HUDFont);
    DrawText(GameTimeString, FColor::White, (ScreenDimensions.X / RIGHTSIDE_X) - GameTimeStringSize.X, (ScreenDimensions.Y / UPSIDE_Y) + ExplosionCountStringSize.Y + EnergyCountStringSize.Y, HUDFont);

    if (MyGameMode->bShowTutorialTimeAttack)
    {
        // Print the tutorial time attack
        FString TutorialCurrentTimeAttackString = FString::Printf(TEXT("Tutorial time : %0.2f"), MyGameMode->TutorialCurrentTimeAttack);
        FVector2D TutorialCurrentTimeAttackStringSize;
        GetTextSize(TutorialCurrentTimeAttackString, TutorialCurrentTimeAttackStringSize.X, TutorialCurrentTimeAttackStringSize.Y, HUDFont);
        DrawText(TutorialCurrentTimeAttackString, FColor::Green, (ScreenDimensions.X / RIGHTSIDE_X) - TutorialCurrentTimeAttackStringSize.X,
            (ScreenDimensions.Y / UPSIDE_Y) + TutorialCurrentTimeAttackStringSize.Y + ExplosionCountStringSize.Y + EnergyCountStringSize.Y, HUDFont);

        // Print the tutorial best time attack
        FString TutorialBestTimeAttackString = FString::Printf(TEXT("Best time : %0.2f"), MyGameMode->TutorialBestTimeAttack);
        FVector2D TutorialBestTimeAttackStringSize;
        GetTextSize(TutorialBestTimeAttackString, TutorialBestTimeAttackStringSize.X, TutorialBestTimeAttackStringSize.Y, HUDFont);
        DrawText(TutorialBestTimeAttackString, FColor::Yellow, (ScreenDimensions.X / RIGHTSIDE_X) - TutorialBestTimeAttackStringSize.X,
            (ScreenDimensions.Y / UPSIDE_Y) + TutorialBestTimeAttackStringSize.Y + ExplosionCountStringSize.Y + EnergyCountStringSize.Y + TutorialCurrentTimeAttackStringSize.Y, HUDFont);
    }    

    if (MyGameMode->bShowExplosionWarning)
    {
        FString WarningString = FString::Printf(TEXT("WARNING!"));
        FVector2D WarningStringSize;
        GetTextSize(WarningString, WarningStringSize.X, WarningStringSize.Y, HUDFont);
        DrawText(WarningString, FColor::Red, (ScreenDimensions.X - WarningStringSize.X) / 2.0f, (ScreenDimensions.Y - WarningStringSize.Y) / 2.3f, HUDFont);

        FString PlanetLifeLossString = FString::Printf(TEXT("-Planet life loss-"));
        FVector2D PlanetLifeLossStringSize;
        GetTextSize(PlanetLifeLossString, PlanetLifeLossStringSize.X, PlanetLifeLossStringSize.Y, HUDFont);
        DrawText(PlanetLifeLossString, FColor::Red, (ScreenDimensions.X - PlanetLifeLossStringSize.X) / 2.0f, (ScreenDimensions.Y - PlanetLifeLossStringSize.Y) / 2.3f + WarningStringSize.Y, HUDFont);
    }
}

void APrototypeHUD::DrawScan()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // If player is currently scanning
    if (MyCharacter->bIsScanning)
    {
        // Scanning HUD
        FString AbsorbEnergyString = FString::Printf(TEXT("Absorbing"));
        FVector2D AbsorbEnergySize;
        GetTextSize(AbsorbEnergyString, AbsorbEnergySize.X, AbsorbEnergySize.Y, HUDFont);
        DrawText(AbsorbEnergyString, FColor::Cyan, (ScreenDimensions.X - AbsorbEnergySize.X) / 2.0f, (ScreenDimensions.Y - AbsorbEnergySize.Y) / 1.75f, HUDFont);

        DrawRect(FColor::Cyan, (ScreenDimensions.X - MyCharacter->ScanProgress * 60.0f * 3.0f) / 2.0f, (ScreenDimensions.Y + 10) / 1.72f, MyCharacter->ScanProgress * 60.0f * 3.0f, 10);
        
        float ScanPourcentage = (MyCharacter->ScanProgress / MyCharacter->ScanMaximum) * 100;
        FString ScanProgressString = FString::Printf(TEXT("(%0.f%)"), ScanPourcentage);
        FVector2D ScanProgressSize;
        GetTextSize(ScanProgressString, ScanProgressSize.X, ScanProgressSize.Y, HUDFont);
        DrawText(ScanProgressString, FColor::Cyan, (ScreenDimensions.X - ScanProgressSize.X) / 2.0f, (ScreenDimensions.Y - ScanProgressSize.Y) / 1.6f, HUDFont);
    }    
}

void APrototypeHUD::DrawGameOver()
{
    // Print the Game over text
    FVector2D GameOverSize;
    GetTextSize(TEXT("Mission failed"), GameOverSize.X, GameOverSize.Y, HUDFont);
    DrawText(TEXT("Mission failed"), FColor::White, (ScreenDimensions.X - GameOverSize.X) / 2.0f, (ScreenDimensions.Y - GameOverSize.Y) / 2.0f, HUDFont);

    // Print the highscore
    FString HighscoreString = FString::Printf(TEXT("Your highscore is %0.0f with %02.0f:%02.0f"), MyGameMode->GetEnergyCount(), floor(MyGameMode->GameTime / 60.0f), fmod(MyGameMode->GameTime, 60.0f));
    FVector2D HighscoreStringSize;
    GetTextSize(HighscoreString, HighscoreStringSize.X, HighscoreStringSize.Y, HUDFont);
    DrawText(HighscoreString, FColor::Yellow, (ScreenDimensions.X - HighscoreStringSize.X) / 2.0f, (ScreenDimensions.Y - HighscoreStringSize.Y) / 2.0f + GameOverSize.Y, HUDFont);

    // Print the options
    FString RestartString = FString::Printf(TEXT("Restart mission (backspace)"));
    FVector2D RestartStringSize;
    GetTextSize(RestartString, RestartStringSize.X, RestartStringSize.Y, HUDFont);
    DrawText(RestartString, FColor::Blue, (ScreenDimensions.X - RestartStringSize.X) / 2.0f, (ScreenDimensions.Y - RestartStringSize.Y) / 2.0f + GameOverSize.Y + HighscoreStringSize.Y, HUDFont);

    FString ExitString = FString::Printf(TEXT("Proceed to the evacuation (escape)"));
    FVector2D ExitStringSize;
    GetTextSize(ExitString, ExitStringSize.X, ExitStringSize.Y, HUDFont);
    DrawText(ExitString, FColor::Blue, (ScreenDimensions.X - ExitStringSize.X) / 2.0f, (ScreenDimensions.Y - ExitStringSize.Y) / 2.0f + GameOverSize.Y + HighscoreStringSize.Y + RestartStringSize.Y, HUDFont);
}

void APrototypeHUD::DrawGameWon()
{
    // Print the Game won text
    FVector2D GameWonSize;
    GetTextSize(TEXT("Mission success!"), GameWonSize.X, GameWonSize.Y, HUDFont);
    DrawText(TEXT("Mission success!"), FColor::White, (ScreenDimensions.X - GameWonSize.X) / 2.0f, (ScreenDimensions.Y - GameWonSize.Y) / 2.0f, HUDFont);

    // Print the options
    FString ContinueString = FString::Printf(TEXT("Continue mission (enter)"));
    FVector2D ContinueStringSize;
    GetTextSize(ContinueString, ContinueStringSize.X, ContinueStringSize.Y, HUDFont);
    DrawText(ContinueString, FColor::Yellow, (ScreenDimensions.X - ContinueStringSize.X) / 2.0f, (ScreenDimensions.Y - ContinueStringSize.Y) / 2.0f + GameWonSize.Y, HUDFont);

    FString RestartString = FString::Printf(TEXT("Restart mission (backspace)"));
    FVector2D RestartStringSize;
    GetTextSize(RestartString, RestartStringSize.X, RestartStringSize.Y, HUDFont);
    DrawText(RestartString, FColor::Blue, (ScreenDimensions.X - RestartStringSize.X) / 2.0f, (ScreenDimensions.Y - RestartStringSize.Y) / 2.0f + GameWonSize.Y + ContinueStringSize.Y, HUDFont);

    FString ExitString = FString::Printf(TEXT("Proceed to the evacuation (escape)"));
    FVector2D ExitStringSize;
    GetTextSize(ExitString, ExitStringSize.X, ExitStringSize.Y, HUDFont);
    DrawText(ExitString, FColor::Blue, (ScreenDimensions.X - ExitStringSize.X) / 2.0f, (ScreenDimensions.Y - ExitStringSize.Y) / 2.0f + GameWonSize.Y + ContinueStringSize.Y + RestartStringSize.Y, HUDFont);
}

