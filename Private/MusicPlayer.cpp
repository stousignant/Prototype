#include "Prototype.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"
#include "Engine.h"

AMusicPlayer::AMusicPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void AMusicPlayer::PlayTutorialMusic()
{
    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    TutorialMusicAC = UGameplayStatics::PlaySoundAttached(TutorialMusic, MyCharacter->GetRootComponent());
    if (TutorialMusicAC)
    {
        TutorialMusicAC->Play();
    }
}

void AMusicPlayer::PlayNormalModeMusic()
{
    // Stop last music
    if (TutorialMusicAC && TutorialMusicAC->IsPlaying())
    {
        TutorialMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    NormalModeMusicAC = UGameplayStatics::PlaySoundAttached(NormalModeMusic, MyCharacter->GetRootComponent());
    if (NormalModeMusicAC)
    {
        NormalModeMusicAC->Play();
    }
}

void AMusicPlayer::PlayHardModeMusic()
{
    // Stop last music
    if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
    {
        GameWonMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    HardModeMusicAC = UGameplayStatics::PlaySoundAttached(HardModeMusic, MyCharacter->GetRootComponent());
    if (HardModeMusicAC)
    {
        HardModeMusicAC->Play();
    }
}

void AMusicPlayer::PlayVeryHardModeMusic()
{
    // Stop last music
    if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
    {
        GameWonMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    VeryHardModeMusicAC = UGameplayStatics::PlaySoundAttached(VeryHardModeMusic, MyCharacter->GetRootComponent());
    if (VeryHardModeMusicAC)
    {
        VeryHardModeMusicAC->Play();
    }
}

void AMusicPlayer::PlayUltimateModeMusic()
{
    // Stop last music
    if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
    {
        GameWonMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    UltimateModeMusicAC = UGameplayStatics::PlaySoundAttached(UltimateModeMusic, MyCharacter->GetRootComponent());
    UltimateModeMusicAC->bAutoDestroy = false;
    if (UltimateModeMusicAC)
    {
        UltimateModeMusicAC->Play();
    }
}

void AMusicPlayer::PlayGameOverMusic()
{
    StopCurrentMusic();

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    GameOverMusicAC = UGameplayStatics::PlaySoundAttached(GameOverMusic, MyCharacter->GetRootComponent());
    if (GameOverMusicAC)
    {
        GameOverMusicAC->Play();
    }    
}

void AMusicPlayer::PlayGameWonMusic()
{
    StopCurrentMusic();

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    GameWonMusicAC = UGameplayStatics::PlaySoundAttached(GameWonMusic, MyCharacter->GetRootComponent());
    if (GameWonMusicAC)
    {
        GameWonMusicAC->Play();
    }
}

void AMusicPlayer::PlayOverloadMusic()
{
    // Interrupt current music
    if (NormalModeMusicAC && NormalModeMusicAC->IsPlaying())
    {
        // Fade out
        NormalModeMusicAC->AdjustVolume(2.0f, 0.0f);
    }
    else if (HardModeMusicAC && HardModeMusicAC->IsPlaying())
    {
        // Fade out
        HardModeMusicAC->AdjustVolume(2.0f, 0.0f);
    }
    else if (VeryHardModeMusicAC && VeryHardModeMusicAC->IsPlaying())
    {
        // Fade out
        VeryHardModeMusicAC->AdjustVolume(2.0f, 0.0f);
    }
    else if (UltimateModeMusicAC && UltimateModeMusicAC->IsPlaying())
    {
        // Fade out
        UltimateModeMusicAC->AdjustVolume(2.0f, 0.0f);
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    // Start overload theme
    OverloadMusicAC = UGameplayStatics::PlaySoundAttached(OverloadMusic, MyCharacter->GetRootComponent());
    if (OverloadMusicAC)
    {
        OverloadMusicAC->Play();
    }
}

void AMusicPlayer::StopOverloadMusic()
{
    // Stop overload music
    if (OverloadMusicAC && OverloadMusicAC->IsPlaying())
    {
        OverloadMusicAC->Stop();
    }
    
    // Playback current music
    if (NormalModeMusicAC && NormalModeMusicAC->IsPlaying())
    {
        // Fade in
        NormalModeMusicAC->AdjustVolume(2.0f, 1.0f);
    }
    else if (HardModeMusicAC && HardModeMusicAC->IsPlaying())
    {
        // Fade in
        HardModeMusicAC->AdjustVolume(2.0f, 1.0f);
    }
    else if (VeryHardModeMusicAC && VeryHardModeMusicAC->IsPlaying())
    {
        // Fade in
        VeryHardModeMusicAC->AdjustVolume(2.0f, 1.0f);
    }
    else if (UltimateModeMusicAC && UltimateModeMusicAC->IsPlaying())
    {
        // Fade in
        UltimateModeMusicAC->AdjustVolume(2.0f, 1.0f);
    }
}

void AMusicPlayer::StopCurrentMusic()
{
    // Stop current music
    if (NormalModeMusicAC && NormalModeMusicAC->IsPlaying())
    {
        NormalModeMusicAC->Stop();
    }
    else if (HardModeMusicAC && HardModeMusicAC->IsPlaying())
    {
        HardModeMusicAC->Stop();
    }
    else if (VeryHardModeMusicAC && VeryHardModeMusicAC->IsPlaying())
    {
        VeryHardModeMusicAC->Stop();
    }
    else if (UltimateModeMusicAC && UltimateModeMusicAC->IsPlaying())
    {
        UltimateModeMusicAC->Stop();
    }
}

/*void AMusicPlayer::PlayMusic(short NewState)
{
    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    switch (NewState)
    {
    //
    //case EPrototypePlayState::ETutorial:
    case 1:
    {
        TutorialMusicAC = UGameplayStatics::PlaySoundAttached(TutorialMusic, MyCharacter->GetRootComponent());
        TutorialMusicAC->Play();
    }
    break;
    //
    //case EPrototypePlayState::EEarlyGame:
    case 2:
    {
        // Stop the current song
        if (TutorialMusicAC && TutorialMusicAC->IsPlaying())
        {
            TutorialMusicAC->Stop();
        }        

        EarlyGameMusicAC = UGameplayStatics::PlaySoundAttached(EarlyGameMusic, MyCharacter->GetRootComponent());
        EarlyGameMusicAC->Play();
    }
    break;
    //
    //case EPrototypePlayState::EGameOver:
    case 3:
    {
        // Stop the current song
        if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
        {
            EarlyGameMusicAC->Stop();
        }
        if (LateGameMusicAC && LateGameMusicAC->IsPlaying())
        {
            LateGameMusicAC->Stop();
        }

        GameOverMusicAC = UGameplayStatics::PlaySoundAttached(GameOverMusic, MyCharacter->GetRootComponent());
        GameOverMusicAC->Play();
    }
    break;
    //
    //case EPrototypePlayState::EGameWon:
    case 4:
    {
        // Stop the current song
        if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
        {
            EarlyGameMusicAC->Stop();
        }

        GameWonMusicAC = UGameplayStatics::PlaySoundAttached(GameWonMusic, MyCharacter->GetRootComponent());
        GameWonMusicAC->Play();
    }
    break;
    //
    //case EPrototypePlayState::ELateGame:
    case 5:
    {
        // Stop the current song
        if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
        {
            GameWonMusicAC->Stop();
        }

        LateGameMusicAC = UGameplayStatics::PlaySoundAttached(LateGameMusic, MyCharacter->GetRootComponent());
        LateGameMusicAC->Play();
    }
    break;
    // 
    //case EPrototypePlayState::EUnknown:
    case 0:
    default:
        // do nothing
        break;
    }
}*/
