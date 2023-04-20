// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundClass.h"

#include "AudioPlayer.generated.h"

/**
 */

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API AAudioPlayer : public AActor
{
	GENERATED_BODY()
public:
	// each channel corresponds to a sound class that controls the configuration of the sound
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, USoundClass*> audioChannels;

	// play audio in an audio channel
	UFUNCTION(BlueprintCallable)
	void PlayAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0);

	// pause the audio channel
	UFUNCTION(BlueprintCallable)
	void PauseAudio(FName channel, float fadeOut = 0);

	// resume the audio channel
	UFUNCTION(BlueprintCallable)
	void ResumeAudio(FName channel, float fadeIn = 0);

	// add audio the audio channel queue
	UFUNCTION(BlueprintCallable)
	void QueueAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0);

	// stop the audio channel and clear all audios from the queue
	UFUNCTION(BlueprintCallable)
	void StopAudio(FName channel, float fadeOut = 0);

protected:
	// Map of audio components for each audio channel
	TMap<FName, UAudioComponent *> AudioComponents;


	struct FAudioInfo
	{
		USoundWave *Audio;
		float FadeIn;
		float FadeOut;
	};
	// Map of queue with fadeIn and fade out for each audio channel
	TMap<FName, TArray<FAudioInfo>> AudioQueue;
};
