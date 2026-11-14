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

	// play audio in an audio channel: it replaces what the channel is playing and clears its queue
	UFUNCTION(BlueprintCallable)
	void PlayAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0, bool loop = false, float volume = 1);

	// pause the audio channel, or every channel when the channel is None
	UFUNCTION(BlueprintCallable)
	void PauseAudio(FName channel, float fadeOut = 0);

	// resume the audio channel, or every channel when the channel is None
	UFUNCTION(BlueprintCallable)
	void ResumeAudio(FName channel, float fadeIn = 0);

	// add audio to the audio channel queue: it starts when the audio the channel is playing
	// reaches its end, so a looping audio never hands over to the next one. Queueing on a
	// channel that is playing nothing starts the audio straight away.
	UFUNCTION(BlueprintCallable)
	void QueueAudio(FName channel, USoundWave *audio, float fadeIn = 0, float fadeOut = 0, bool loop = false, float volume = 1);

	// play a short sound through a channel's sound class without taking the channel over: the
	// audio the channel is playing, its fades and its queue are all left alone, so interface
	// and lip-sync sounds cannot cut off audio the script started. Each channel has a single
	// one-shot slot, so a new one-shot replaces the one before it instead of piling up.
	UFUNCTION(BlueprintCallable)
	void PlayOneShot(FName channel, USoundWave *audio, float volume = 1);

	// stop the audio channel and clear all audios from the queue
	UFUNCTION(BlueprintCallable)
	void StopAudio(FName channel, float fadeOut = 0);

protected:
	// Map of audio components for each audio channel
	UPROPERTY()
	UWorld *audioWorld;
	UPROPERTY()
	TMap<FName, UAudioComponent *> audioComponents;

	// Weak: nothing roots the player by default, so GC may collect it once its widgets are gone.
	static TWeakObjectPtr<UAudioPlayer> singletonInstance;
	struct FAudioInfo
	{
		// Weak: the script owns the sound asset, waiting in a queue must not keep it alive.
		TWeakObjectPtr<USoundWave> Audio;
		float FadeIn = 0;
		float FadeOut = 0;
		float Volume = 1;
		bool loop = false;
	};
	// Map of queue with fadeIn and fade out for each audio channel
	TMap<FName, TArray<FAudioInfo>> audioQueues;

	// Start audio on a slot of audioComponents and ask to be told when it ends, so that the
	// slot's queue can advance. Returns the component, or null when it could not be started.
	UAudioComponent *StartSlotAudio(FName slot, USoundClass *soundClass, const FAudioInfo &info);
	// Stop what a slot is playing because something asked for it rather than because the audio
	// ended: the component is forgotten first, so the finish callback raised by the stop cannot
	// be mistaken for the audio reaching its end.
	void StopSlotAudio(FName slot, float fadeOut);
	// Start the next audio queued on a channel once the audio it was playing has ended.
	void OnSlotAudioFinished(UAudioComponent *component);
	// The slot a channel's one-shots play on, kept apart from the channel's own audio.
	static FName OneShotSlot(FName channel);
};
