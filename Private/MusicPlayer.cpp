#include "Prototype.h"
#include "PrototypeCharacter.h"
#include "MusicPlayer.h"

AMusicPlayer::AMusicPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AMusicPlayer::PlayMusic(EPrototypePlayState NewState)
{
    // Get the character
    APrototypeCharacter* MyCharacter = Cast<APrototypeCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

    switch (NewState)
    {
    //
    case EPrototypePlayState::ETutorial:
    {
        TutorialMusicAC = UGameplayStatics::PlaySoundAttached(TutorialMusic, MyCharacter->GetRootComponent());
        TutorialMusicAC->Play();
    }
    break;
    //
    case EPrototypePlayState::EEarlyGame:
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
    case EPrototypePlayState::EGameOver:
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
    case EPrototypePlayState::EGameWon:
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
    case EPrototypePlayState::ELateGame:
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
    case EPrototypePlayState::EUnknown:
    default:
        // do nothing
        break;
    }
}






