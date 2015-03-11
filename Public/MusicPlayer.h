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

    // Early Game song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* EarlyGameMusic;

    /** Used to manipulate with the early game music */
    UPROPERTY()
    UAudioComponent* EarlyGameMusicAC;

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

    // Late Game song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* LateGameMusic;

    /** Used to manipulate with the late game music */
    UPROPERTY()
    UAudioComponent* LateGameMusicAC;
	
	// Play a song
    void PlayMusic(EPrototypePlayState NewState);
	
	
};
