#include "AudioPlayer.h"
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"

void AAudioPlayer::PlayAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut)
{
	// Check if the audio channel exists
	if (audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Display, TEXT("AAudioPlayer::PlayAudio %s"), *channel.ToString());
		// Create an audio component if it doesn't exist
		if (!AudioComponents.Contains(channel))
		{
			UAudioComponent *newComponent = NewObject<UAudioComponent>(this);
			// UAudioComponent *newComponent = SpawnSound2D(this, audio);
			newComponent->RegisterComponent();
			newComponent->SetUISound(true);
			newComponent->SoundClassOverride = audioChannels[channel];
			AudioComponents.Add(channel, newComponent);
		}

		UAudioComponent *audioComponent = AudioComponents[channel];

		// Set the audio and fade parameters
		audioComponent->SetSound(audio);

		// Play the audio
		audioComponent->FadeIn(fadeIn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed AAudioPlayer::PlayAudio %s"), *channel.ToString());
	}
}

void AAudioPlayer::PauseAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing
	if (AudioComponents.Contains(channel) && AudioComponents[channel]->IsPlaying())
	{
		UE_LOG(LogTemp, Display, TEXT("AAudioPlayer::PauseAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = AudioComponents[channel];

		// Fade out and pause the audio
		audioComponent->FadeOut(fadeOut, 0);
		audioComponent->SetPaused(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed AAudioPlayer::PauseAudio %s"), *channel.ToString());
	}
}

void AAudioPlayer::ResumeAudio(FName channel, float fadeIn)
{
	// Check if the audio component exists and is paused
	if (AudioComponents.Contains(channel) && AudioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Paused)
	{
		UE_LOG(LogTemp, Display, TEXT("AAudioPlayer::ResumeAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = AudioComponents[channel];

		// Fade in and resume the audio
		audioComponent->FadeIn(fadeIn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed AAudioPlayer::ResumeAudio %s"), *channel.ToString());
	}
}

void AAudioPlayer::QueueAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut)
{
	// Check if the audio channel exists
	if (audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Display, TEXT("AAudioPlayer::QueueAudio %s"), *channel.ToString());
		// Add the audio info to the queue
		FAudioInfo audioInfo;
		audioInfo.Audio = audio;
		audioInfo.FadeIn = fadeIn;
		audioInfo.FadeOut = fadeOut;

		AudioQueue.FindOrAdd(channel).Add(audioInfo);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed AAudioPlayer::QueueAudio %s"), *channel.ToString());
	}
}

void AAudioPlayer::StopAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing or paused
	if (AudioComponents.Contains(channel) &&
			(AudioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Playing || AudioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Paused))
	{
		UE_LOG(LogTemp, Display, TEXT("AAudioPlayer::StopAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = AudioComponents[channel];

		// Fade out and stop the audio
		audioComponent->FadeOut(fadeOut, 0);

		// Clear the audio queue
		AudioQueue.FindOrAdd(channel).Empty();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed AAudioPlayer::StopAudio %s"), *channel.ToString());
	}
}
