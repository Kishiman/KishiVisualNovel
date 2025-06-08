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
		if (AudioComponents.Contains(channel))
		{
			UAudioComponent *audioComponent = AudioComponents[channel];

			if (audioComponent)
			{
				// This line keeps returning error
				/*
				Unhandled Exception: EXCEPTION_ACCESS_VIOLATION reading address 0x0000000000000648

				UE4Editor_KishiVisualNovel!UAudioPlayer::PlayAudio() [D:\Projects\Unreal\JailBrain\Plugins\KishiVisualNovel\Source\KishiVisualNovel\Private\AudioPlayer.cpp:30]
				UE4Editor_KishiVisualNovel!UAudioPlayer::execPlayAudio() [D:\Projects\Unreal\JailBrain\Plugins\KishiVisualNovel\Intermediate\Build\Win64\UE4Editor\Inc\KishiVisualNovel\AudioPlayer.gen.cpp:73]
				*/
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
		AudioComponents.Add(channel, newComponent);

		UAudioComponent *audioComponent = newComponent;

		if (!audioComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayAudio(%s): audioComponent is null"), *channel.ToString());
			return;
		}

		// Clear the audio queue
		AudioQueue.FindOrAdd(channel).Empty();

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

		AudioQueue.FindOrAdd(channel).Add(audioInfo);
	}
	catch (const std::exception &e)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::QueueAudio(%s): Caught exception: %s"), *channel.ToString(), e.what());
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
