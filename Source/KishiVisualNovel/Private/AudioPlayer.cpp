#include "AudioPlayer.h"
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"

void UAudioPlayer::PlayAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut, bool loop)
{
	// Check if the audio channel exists
	if (audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::PlayAudio %s"), *channel.ToString());
		// Create an audio component if it doesn't exist
		if (!AudioComponents.Contains(channel))
		{
			UAudioComponent *newComponent = UGameplayStatics::SpawnSound2D(this, audio);
			newComponent->SetUISound(true);
			newComponent->SoundClassOverride = audioChannels[channel];
			AudioComponents.Add(channel, newComponent);
		}

		UAudioComponent *audioComponent = AudioComponents[channel];

		// Clear the audio queue
		AudioQueue.FindOrAdd(channel).Empty();

		// Enable looping
		audio->bLooping = loop ? 1 : 0;

		// Set the audio and fade parameters
		audioComponent->SetSound(audio);

		// Play the audio
		audioComponent->FadeIn(fadeIn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::PlayAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::PauseAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing
	if (AudioComponents.Contains(channel) && AudioComponents[channel]->IsPlaying())
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::PauseAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = AudioComponents[channel];

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
	if (AudioComponents.Contains(channel) && AudioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Paused)
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::ResumeAudio %s"), *channel.ToString());
		UAudioComponent *audioComponent = AudioComponents[channel];

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
	// Check if the audio channel exists
	if (audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::QueueAudio %s"), *channel.ToString());
		// Add the audio info to the queue
		FAudioInfo audioInfo;
		audioInfo.Audio = audio;
		audioInfo.FadeIn = fadeIn;
		audioInfo.FadeOut = fadeOut;
		audioInfo.loop = loop;

		AudioQueue.FindOrAdd(channel).Add(audioInfo);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::QueueAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::StopAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing or paused
	if (AudioComponents.Contains(channel))
	{
		auto PlayState = AudioComponents[channel]->GetPlayState();
		if (PlayState != EAudioComponentPlayState::Stopped)
		{
			UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::StopAudio %s"), *channel.ToString());
			UAudioComponent *audioComponent = AudioComponents[channel];

			// Fade out and stop the audio
			audioComponent->FadeOut(fadeOut, 0);

			// Clear the audio queue
			AudioQueue.FindOrAdd(channel).Empty();
			return;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::StopAudio %s"), *channel.ToString());
}
