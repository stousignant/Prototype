#include "Prototype.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"

AMusicPlayer::AMusicPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bInterruptedEarlyGameMusic = false;
    bInterruptedLateGameMusic = false;
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

void AMusicPlayer::PlayEarlyGameMusic()
{
    // Stop last music
    if (TutorialMusicAC && TutorialMusicAC->IsPlaying())
    {
        TutorialMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    EarlyGameMusicAC = UGameplayStatics::PlaySoundAttached(EarlyGameMusic, MyCharacter->GetRootComponent());
    if (EarlyGameMusicAC)
    {
        EarlyGameMusicAC->Play();
    }
}

void AMusicPlayer::PlayGameWonMusic()
{
    // Stop last music
    if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
    {
        EarlyGameMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    GameWonMusicAC = UGameplayStatics::PlaySoundAttached(GameWonMusic, MyCharacter->GetRootComponent());
    if (GameWonMusicAC)
    {
        GameWonMusicAC->Play();
    }
}

void AMusicPlayer::PlayLateGameMusic()
{
    // Stop last music
    if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
    {
        GameWonMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    LateGameMusicAC = UGameplayStatics::PlaySoundAttached(LateGameMusic, MyCharacter->GetRootComponent());
    if (LateGameMusic)
    {
        LateGameMusicAC->Play();
    }
}

void AMusicPlayer::PlayGameOverMusic()
{
    // Stop last music
    if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
    {
        EarlyGameMusicAC->Stop();
    }
    else if (LateGameMusicAC && LateGameMusicAC->IsPlaying())
    {
        LateGameMusicAC->Stop();
    }

    // Get the character
    MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    //
    GameOverMusicAC = UGameplayStatics::PlaySoundAttached(GameOverMusic, MyCharacter->GetRootComponent());
    if (GameOverMusicAC)
    {
        GameOverMusicAC->Play();
    }    
}

void AMusicPlayer::PlayOverloadMusic()
{
    // Interrupt current music
    if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
    {
        EarlyGameMusicAC->Stop();
        bInterruptedEarlyGameMusic = true;
    }
    else if (LateGameMusicAC && LateGameMusicAC->IsPlaying())
    {
        LateGameMusicAC->Stop();
        bInterruptedLateGameMusic = true;
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

    // Restart interrupted music
    if (bInterruptedEarlyGameMusic && EarlyGameMusicAC)
    {
        EarlyGameMusicAC->Play();
    }
    else if (bInterruptedLateGameMusic && LateGameMusicAC)
    {
        LateGameMusicAC->Play();
    }
}

/*void AMusicPlayer::StopAllMusic()
{
    // Stop the current song
    if (TutorialMusicAC && TutorialMusicAC->IsPlaying())
    {
        TutorialMusicAC->Stop();
    }
    else if (EarlyGameMusicAC && EarlyGameMusicAC->IsPlaying())
    {
        EarlyGameMusicAC->Stop();
    }
    else if (GameWonMusicAC && GameWonMusicAC->IsPlaying())
    {
        GameWonMusicAC->Stop();
    }
    else if (LateGameMusicAC && LateGameMusicAC->IsPlaying())
    {
        LateGameMusicAC->Stop();
    }
    else if (GameOverMusicAC && GameOverMusicAC->IsPlaying())
    {
        GameOverMusicAC->Stop();
    }
    else if (OverloadMusicAC && OverloadMusicAC->IsPlaying())
    {
        OverloadMusicAC->Stop();
    }
}*/

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
