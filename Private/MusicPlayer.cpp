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
    case EPrototypePlayState::EPlaying:
    {
        TutorialMusicAC->Stop();

        GameMusicAC = UGameplayStatics::PlaySoundAttached(GameMusic, MyCharacter->GetRootComponent());
        GameMusicAC->Play();
    }
    break;
    //
    case EPrototypePlayState::EGameOver:
    {
        GameMusicAC->Stop();

        EndingMusicAC = UGameplayStatics::PlaySoundAttached(EndingMusic, MyCharacter->GetRootComponent());
        EndingMusicAC->Play();
    }
    break;
    // 
    case EPrototypePlayState::EUnknown:
    default:
        // do nothing
        break;
    }
}






