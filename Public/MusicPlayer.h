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

    // - TUTORIAL -
    /** Tutorial mode song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* TutorialMusic;

    /** Used to manipulate with the tutorial music */
    UPROPERTY()
    UAudioComponent* TutorialMusicAC;

    /** Function to play the tutorial mode music */
    void PlayTutorialMusic();

    // - NORMAL MODE -
    /** Normal mode song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* NormalModeMusic;

    /** Used to manipulate with the early game music */
    UPROPERTY()
    UAudioComponent* NormalModeMusicAC;

    /** Function to play the normal mode music */
    void PlayNormalModeMusic();

    // - HARD MODE -
    /** Hard mode song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* HardModeMusic;

    /** Used to manipulate with the hard mode music */
    UPROPERTY()
    UAudioComponent* HardModeMusicAC;

    /** Function to play the hard mode music */
    void PlayHardModeMusic();

    // - VERY HARD MODE -
    /** Very hard mode song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* VeryHardModeMusic;

    /** Used to manipulate with the very hard mode music */
    UPROPERTY()
    UAudioComponent* VeryHardModeMusicAC;

    /** Function to play the very hard mode music */
    void PlayVeryHardModeMusic();

    // - ULTIMATE MODE -
    /** Ultimate mode song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* UltimateModeMusic;

    /** Used to manipulate with the ultimate mode music */
    UPROPERTY()
    UAudioComponent* UltimateModeMusicAC;

    /** Function to play the ultimate mode music */
    void PlayUltimateModeMusic();

    // - GAME OVER -
    /** Game over song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameOverMusic;

    /** Used to manipulate with the game over music */
    UPROPERTY()
    UAudioComponent* GameOverMusicAC;

    /** Function to play the game over music */
    void PlayGameOverMusic();

    // - GAME WON -
    /** Game won song */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Music)
    class USoundBase* GameWonMusic;

    /** Used to manipulate with the game won music */
    UPROPERTY()
    UAudioComponent* GameWonMusicAC;

    /** Function to play the game won music */
    void PlayGameWonMusic();

    

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
    
    // Stop the current music
    void StopCurrentMusic();

    // Play a song
    //void PlayMusic(short NewState);
};
