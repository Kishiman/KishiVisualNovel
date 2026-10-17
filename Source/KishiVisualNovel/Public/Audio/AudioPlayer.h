// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundClass.h"

#include "AudioPlayer.generated.h"

/**
 */

UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UAudioPlayer : public UObject
{
	GENERATED_BODY()
public:
	// each channel corresponds to a sound class that controls the configuration of the sound
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, USoundClass *> audioChannels;

	virtual void PostInitProperties() override;

	void OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ShutdownAudioPlayer();

	// get the singleton instance of the audio player
	UFUNCTION(BlueprintCallable)
	static UAudioPlayer *GetSingletonInstance(UObject *Outer, bool bRooted = false);

	// play audio in an audio channel
	UFUNCTION(BlueprintCallable)
	void PlayAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0, bool loop = false);

	// pause the audio channel
	UFUNCTION(BlueprintCallable)
	void PauseAudio(FName channel, float fadeOut = 0);

	// resume the audio channel
	UFUNCTION(BlueprintCallable)
	void ResumeAudio(FName channel, float fadeIn = 0);

	// add audio the audio channel queue
	UFUNCTION(BlueprintCallable)
	void QueueAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0, bool loop = false);

	// stop the audio channel and clear all audios from the queue
	UFUNCTION(BlueprintCallable)
	void StopAudio(FName channel, float fadeOut = 0);

protected:
	// Map of audio components for each audio channel
	UPROPERTY()
	UWorld *audioWorld;
	UPROPERTY()
	TMap<FName, UAudioComponent *> audioComponents;

	static UAudioPlayer *singletonInstance;
	struct FAudioInfo
	{
		USoundWave *Audio;
		float FadeIn;
		float FadeOut;
		bool loop;
	};
	// Map of queue with fadeIn and fade out for each audio channel
	TMap<FName, TArray<FAudioInfo>> audioQueues;
};
