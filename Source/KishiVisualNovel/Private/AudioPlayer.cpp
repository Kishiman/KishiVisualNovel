#include "AudioPlayer.h"
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"

void UAudioPlayer::PlayAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut, bool loop)
{
	// Check if the audio not null
	if (!audio || audio->IsPendingKill())
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): NULL or invalid audio"), *channel.ToString());
		return;
	}
	// Check if the audio channel exists
	if (!audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): channel does not exist "), *channel.ToString());
		return;
	}
	try
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::PlayAudio %s"), *channel.ToString());
		// Create an audio component if it doesn't exist
		if (audioComponents.Contains(channel))
		{
			UAudioComponent *audioComponent = audioComponents[channel];

			if (audioComponent)
			{
				audioComponent->FadeOut(fadeOut, 0);
			}
		}
		UAudioComponent *newComponent = UGameplayStatics::SpawnSound2D(this, audio);
		if (!newComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): Failed to spawn sound component"), *channel.ToString());
			return;
		}
		newComponent->SetUISound(true);
		newComponent->SoundClassOverride = audioChannels[channel];
		audioComponents.Add(channel, newComponent);

		UAudioComponent *audioComponent = newComponent;

		if (!audioComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): audioComponent is null"), *channel.ToString());
			return;
		}

		// Clear the audio queue
		audioQueues.FindOrAdd(channel).Empty();

		// if (!audio->load())
		// {
		// 	audio->ConditionalPostLoad();
		// 	UE_LOG(LogTemp, Warning, TEXT("UAudioPlayer::PlayAudio(%s): Forcing load of audio asset"), *channel.ToString());
		// }
		// Enable looping
		audio->bLooping = loop ? 1 : 0;

		// UGameplayStatics::PlaySound2D(this, audio);
		// return;
		// Set the audio and fade parameters
		audioComponent->SetSound(audio);

		if (!audioComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): audioComponent is null"), *channel.ToString());
			return;
		}
		if (audioComponent && !(audioComponent->Sound))
		{
			UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): Failed to set audio"), *channel.ToString());
			return;
		}

		// Play the audio
		audioComponent->FadeIn(fadeIn);
	}
	catch (const std::exception &e)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): Caught exception: %s"), *channel.ToString(), e.what());
	}
}

void UAudioPlayer::PauseAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing
	if (audioComponents.Contains(channel) && audioComponents[channel]->IsPlaying())
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::PauseAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = audioComponents[channel];

		// Fade out and pause the audio
		audioComponent->FadeOut(fadeOut, 0);
		audioComponent->SetPaused(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::PauseAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::ResumeAudio(FName channel, float fadeIn)
{
	// Check if the audio component exists and is paused
	if (audioComponents.Contains(channel) && audioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Paused)
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::ResumeAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = audioComponents[channel];

		// Fade in and resume the audio
		audioComponent->FadeIn(fadeIn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::ResumeAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::QueueAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut, bool loop)
{
	// Check if the audio not null
	if (!audio || audio->IsPendingKill())
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): NULL or invalid audio"), *channel.ToString());
		return;
	}
	// Check if the audio channel exists
	if (!audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::QueueAudio(%s): channel does not exist "), *channel.ToString());
		return;
	}
	try
	{
		/* code */
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::QueueAudio %s"), *channel.ToString());
		// Add the audio info to the queue
		FAudioInfo audioInfo;
		audioInfo.Audio = audio;
		audioInfo.FadeIn = fadeIn;
		audioInfo.FadeOut = fadeOut;
		audioInfo.loop = loop;

		audioQueues.FindOrAdd(channel).Add(audioInfo);
	}
	catch (const std::exception &e)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::QueueAudio(%s): Caught exception: %s"), *channel.ToString(), e.what());
	}
}

void UAudioPlayer::StopAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing or paused
	if (audioComponents.Contains(channel))
	{
		auto PlayState = audioComponents[channel]->GetPlayState();
		if (PlayState != EAudioComponentPlayState::Stopped)
		{
			UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::StopAudio %s"), *channel.ToString());
			UAudioComponent *audioComponent = audioComponents[channel];

			// Fade out and stop the audio
			audioComponent->FadeOut(fadeOut, 0);

			// Clear the audio queue
			audioQueues.FindOrAdd(channel).Empty();
			return;
		}
	}
}

UAudioPlayer *UAudioPlayer::singletonInstance = nullptr;

void UAudioPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	// Only bind once
	if (!FWorldDelegates::OnWorldCleanup.IsBoundToObject(this))
	{
		FWorldDelegates::OnWorldCleanup.AddUObject(this, &UAudioPlayer::OnWorldCleanup);
	}
}
void UAudioPlayer::OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources)
{
	if (this->audioWorld != World)
		return;
	this->ShutdownAudioPlayer();
}

void UAudioPlayer::ShutdownAudioPlayer()
{

	UE_LOG(LogTemp, Log, TEXT("UAudioPlayer: Shutting down instance."));

	// Step 1. Stop and fade out all active audio components
	for (auto &Pair : this->audioComponents)
	{
		if (UAudioComponent *AudioComp = Pair.Value)
		{
			if (AudioComp->IsPlaying())
			{
				// Smoothly fade out before stopping (optional)
				const float FadeOutDuration = 0.5f;
				AudioComp->FadeOut(FadeOutDuration, 0.0f);

				// Mark for destruction after fade out
				AudioComp->bAutoDestroy = true;
			}
		}
	}

	// Optionally clear all channels and audio components
	this->audioQueues.Empty();
	this->audioChannels.Empty();
	this->audioComponents.Empty();

	// Remove from root so GC can reclaim it
	if (this->IsRooted())
	{
		this->RemoveFromRoot();
		this->ConditionalBeginDestroy();
	}
	// Force destruction (safe only if not referenced elsewhere)
	if (this == singletonInstance)
	{
		singletonInstance = nullptr;
	}
}

UAudioPlayer *UAudioPlayer::GetSingletonInstance(UObject *Outer, bool bRooted)
{
	if (!singletonInstance)
	{
		singletonInstance = NewObject<UAudioPlayer>(Outer);
		singletonInstance->audioWorld = Outer->GetWorld();
	}
	if (bRooted && !singletonInstance->IsRooted())
	{
		singletonInstance->AddToRoot();
	}
	else if (!bRooted && singletonInstance->IsRooted())
	{
		singletonInstance->RemoveFromRoot();
	}
	return singletonInstance;
}
