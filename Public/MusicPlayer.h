#pragma once

#include "PrototypeGameMode.h"
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

    // Tutorial song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* TutorialMusic;

    /** Used to manipulate with the tutorial music */
    UPROPERTY()
    UAudioComponent* TutorialMusicAC;

    // Game song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* PlayingMusic;

    /** Used to manipulate with the game music */
    UPROPERTY()
        UAudioComponent* PlayingMusicAC;

    // Game over song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameOverMusic;

    /** Used to manipulate with the game over music */
    UPROPERTY()
    UAudioComponent* GameOverMusicAC;

    // Game won song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameWonMusic;

    /** Used to manipulate with the game won music */
    UPROPERTY()
    UAudioComponent* GameWonMusicAC;
	
	// Play a song
    void PlayMusic(EPrototypePlayState NewState);
	
	
};
