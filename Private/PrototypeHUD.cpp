// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Prototype.h"
#include "PrototypeHUD.h"
#include "PrototypeGameMode.h"
#include "PrototypeCharacter.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"

APrototypeHUD::APrototypeHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Use the RobotoDistanceField font from the engine
    static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = HUDFontOb.Object;

    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/Textures/Crosshair"));
    CrosshairTex = CrosshairTexObj.Object;
}


void APrototypeHUD::DrawHUD()
{
    // Parent call
	Super::DrawHUD();

    // Get the screen dimensions
    ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);
    
    DrawPlayerInfo();

    DrawPlayerPowers();

    DrawGameInfo();

    //DrawCrosshair();

    DrawScan();
    
    DrawGameState();
    
}

void APrototypeHUD::DrawPlayerInfo()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Print speed
    FString SpeedString = FString::Printf(TEXT("Speed  %0.f"), MyCharacter->GetVelocity().Size());
    FVector2D SpeedStringSize;
    GetTextSize(SpeedString, SpeedStringSize.X, SpeedStringSize.Y, HUDFont);
    DrawText(SpeedString, FColor::Cyan, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 3.0f), HUDFont);

    // Print fake speed
    //FString FakeSpeedString = FString::Printf(TEXT("FSpeed%1.1f km/h"), MyCharacter->GetVelocity().Size() / 50);
    //FVector2D FakeSpeedStringSize;
    //GetTextSize(FakeSpeedString, FakeSpeedStringSize.X, FakeSpeedStringSize.Y, HUDFont);
    //DrawText(FakeSpeedString, FColor::Cyan, (ScreenDimensions.X / 2.7f), (ScreenDimensions.Y / 2.8f), HUDFont);

    // Print stamina
    FString StaminaString = FString::Printf(TEXT("Stamina %0.f"), MyCharacter->StaminaCurrent);
    FVector2D StaminaStringSize;
    GetTextSize(StaminaString, StaminaStringSize.X, StaminaStringSize.Y, HUDFont);
    DrawText(StaminaString, FColor::Green, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 3.0f) + SpeedStringSize.Y, HUDFont);

    if (MyCharacter->bIsDead)
    {
        FString RespawnString = FString::Printf(TEXT("Rebooting"));
        FVector2D RespawnStringSize;
        GetTextSize(RespawnString, RespawnStringSize.X, RespawnStringSize.Y, HUDFont);
        DrawText(RespawnString, FColor::Cyan, (ScreenDimensions.X - RespawnStringSize.X) / 2.0f, (ScreenDimensions.Y - RespawnStringSize.Y) / 2.3f, HUDFont);
    }
}

void APrototypeHUD::DrawPlayerPowers()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Color of the text based on the unspent stats
    FColor ColorToDisplay = (MyCharacter->StatsCount > 0) ? FColor::Blue : FColor::White;

    // Print stamina power
    FString StaminaLevelString = FString::Printf(TEXT("Stamina Lv.%0.f"), MyCharacter->StaminaPowerLevel);
    FVector2D StaminaLevelStringSize;
    GetTextSize(StaminaLevelString, StaminaLevelStringSize.X, StaminaLevelStringSize.Y, HUDFont);
    DrawText(StaminaLevelString, ColorToDisplay, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 1.5f), HUDFont);

    // Print jump power
    FString JumpLevelString = FString::Printf(TEXT("Jump Lv.%0.f"), MyCharacter->JumpPowerLevel);
    FVector2D JumpLevelStringSize;
    GetTextSize(JumpLevelString, JumpLevelStringSize.X, JumpLevelStringSize.Y, HUDFont);
    DrawText(JumpLevelString, ColorToDisplay, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 1.5f) - StaminaLevelStringSize.Y, HUDFont);

    // Print speed power
    FString SpeedLevelString = FString::Printf(TEXT("Speed Lv.%0.f"), MyCharacter->SpeedPowerLevel);
    FVector2D SpeedLevelStringSize;
    GetTextSize(SpeedLevelString, SpeedLevelStringSize.X, SpeedLevelStringSize.Y, HUDFont);
    DrawText(SpeedLevelString, ColorToDisplay, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 1.5f) - StaminaLevelStringSize.Y - JumpLevelStringSize.Y, HUDFont);

    // Print stats count
    if (MyCharacter->StatsCount > 0)
    {
        FString StatsCountString = FString::Printf(TEXT("Unspent Stats %0.f"), MyCharacter->StatsCount);
        FVector2D StatsCountStringSize;
        GetTextSize(StatsCountString, StatsCountStringSize.X, StatsCountStringSize.Y, HUDFont);
        DrawText(StatsCountString, ColorToDisplay, (ScreenDimensions.X / 3.2f), (ScreenDimensions.Y / 1.5f) - StaminaLevelStringSize.Y - JumpLevelStringSize.Y - SpeedLevelStringSize.Y, HUDFont);
    }    
}

void APrototypeHUD::DrawGameInfo()
{
    // Get the game mode
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    
    // Print the energy count
    FString EnergyCountString = FString::Printf(TEXT("%0.0f Energy"), MyGameMode->EnergyCount);
    FVector2D EnergyCountStringSize;
    GetTextSize(EnergyCountString, EnergyCountStringSize.X, EnergyCountStringSize.Y, HUDFont);
    DrawText(EnergyCountString, FColor::Yellow, (ScreenDimensions.X / 1.6f), (ScreenDimensions.Y / 3.0f), HUDFont);
}

void APrototypeHUD::DrawCrosshair()
{
    // Find center of the Canvas
    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

    // Offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
    const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)), (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));

    // Draw the crosshair
    FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(TileItem);
}

void APrototypeHUD::DrawScan()
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // If player is currently scanning
    if (MyCharacter->bIsScanning)
    {
        // Scanning HUD
        float ScanPourcentage = (MyCharacter->ScanProgress / MyCharacter->ScanMaximum) * 100;
        FString ScanProgressString = FString::Printf(TEXT("Scanning (%.1f)"), ScanPourcentage);
        FVector2D ScanProgressSize;
        GetTextSize(ScanProgressString, ScanProgressSize.X, ScanProgressSize.Y, HUDFont);
        DrawText(ScanProgressString, FColor::Cyan, (ScreenDimensions.X - ScanProgressSize.X) / 2.0f, (ScreenDimensions.Y - ScanProgressSize.Y) / 1.7f, HUDFont);
        DrawRect(FColor::Cyan, (ScreenDimensions.X - MyCharacter->ScanProgress) / 2.0f, (ScreenDimensions.Y + 10) / 1.65f, MyCharacter->ScanProgress, 10);
    }    
}

void APrototypeHUD::DrawGameState()
{
    // If the game is over 
    APrototypeGameMode* MyGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(this));
    if (MyGameMode->GetCurrentState() == EPrototypePlayState::EGameOver)
    {
        // Create a variable for storing the size of printing Game Over
        FVector2D GameOverSize;
        GetTextSize(TEXT("GAME OVER"), GameOverSize.X, GameOverSize.Y, HUDFont);
        DrawText(TEXT("GAME OVER"), FColor::White, (ScreenDimensions.X - GameOverSize.X) / 2.0f, (ScreenDimensions.Y - GameOverSize.Y) / 2.0f, HUDFont);
    }
}

