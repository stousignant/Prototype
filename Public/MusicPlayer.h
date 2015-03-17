#pragma once

#include "PrototypeCharacter.h"
#include "GameFramework/Actor.h"
#include "MusicPlayer.generated.h"


/**
 * 
 */
UCLASS()
class PROTOTYPE_API AMusicPlayer : public AActor
{
	GENERATED_BODY()

public:

    // Constructor
    AMusicPlayer(const FObjectInitializer& ObjectInitializer);

    // Character reference
    APrototypeCharacter* MyCharacter;

    // Tutorial song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* TutorialMusic;

    /** Used to manipulate with the tutorial music */
    UPROPERTY()
    UAudioComponent* TutorialMusicAC;

    //
    void PlayTutorialMusic();

    // Early Game song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* EarlyGameMusic;

    /** Used to manipulate with the early game music */
    UPROPERTY()
    UAudioComponent* EarlyGameMusicAC;

    //
    void PlayEarlyGameMusic();

    // Game over song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameOverMusic;

    /** Used to manipulate with the game over music */
    UPROPERTY()
    UAudioComponent* GameOverMusicAC;

    //
    void PlayGameOverMusic();

    // Game won song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameWonMusic;

    /** Used to manipulate with the game won music */
    UPROPERTY()
    UAudioComponent* GameWonMusicAC;

    //
    void PlayGameWonMusic();

    // Late Game song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* LateGameMusic;

    /** Used to manipulate with the late game music */
    UPROPERTY()
    UAudioComponent* LateGameMusicAC;

    //
    void PlayLateGameMusic();

    // Overload
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* OverloadMusic;

    // 
    UPROPERTY()
    UAudioComponent* OverloadMusicAC;

    // Play overload music
    void PlayOverloadMusic();

    // Play overload music
    void StopOverloadMusic();

    //
    bool bInterruptedEarlyGameMusic;

    //
    bool bInterruptedLateGameMusic;

    //
    //void StopAllMusic();

    // Play a song
    //void PlayMusic(short NewState);
};
