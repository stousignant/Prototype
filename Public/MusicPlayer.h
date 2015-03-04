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
    class USoundBase* GameMusic;

    /** Used to manipulate with the game music */
    UPROPERTY()
    UAudioComponent* GameMusicAC;

    // Ending song
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* EndingMusic;

    /** Used to manipulate with the ending music */
    UPROPERTY()
    UAudioComponent* EndingMusicAC;
	
	// Play a song
    void PlayMusic(EPrototypePlayState NewState);
	
	
};
