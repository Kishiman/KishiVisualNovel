#include "Audio/AudioPlayer.h"
#include "Components/AudioComponent.h"

#include "Kismet/GameplayStatics.h"

FName UAudioPlayer::OneShotSlot(FName channel)
{
	// A derived key, so a one-shot and the audio a channel itself plays never share a slot.
	return FName(*FString::Printf(TEXT("%s (one-shot)"), *channel.ToString()));
}

UAudioComponent *UAudioPlayer::StartSlotAudio(FName slot, USoundClass *soundClass, const FAudioInfo &info)
{
	USoundWave *audio = info.Audio.Get();
	if (!audio)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::StartSlotAudio(%s): the audio is no longer loaded"), *slot.ToString());
		return nullptr;
	}
	// The sound class is an override on the component, and it is only read when playback starts,
	// so the component is created stopped and played once the channel's class is on it.
	UAudioComponent *component = UGameplayStatics::CreateSound2D(this, audio);
	if (!component)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::StartSlotAudio(%s): Failed to create sound component"), *slot.ToString());
		return nullptr;
	}
	component->SetUISound(true);
	component->SoundClassOverride = soundClass;
	audioComponents.Add(slot, component);
	component->OnAudioFinishedNative.AddUObject(this, &UAudioPlayer::OnSlotAudioFinished);
	// 4.27 has no per-component loop flag: looping is a property of the wave itself.
	audio->bLooping = info.loop ? 1 : 0;
	// A zero duration sets the volume straight away instead of fading up to it.
	component->FadeIn(info.FadeIn, info.Volume);
	return component;
}

void UAudioPlayer::StopSlotAudio(FName slot, float fadeOut)
{
	UAudioComponent *component = nullptr;
	audioComponents.RemoveAndCopyValue(slot, component);
	if (IsValid(component))
	{
		component->FadeOut(fadeOut, 0.0f);
	}
}

void UAudioPlayer::OnSlotAudioFinished(UAudioComponent *component)
{
	if (!component)
	{
		return;
	}
	// Only the component a slot is still playing advances that slot's queue: one that was
	// replaced or stopped on purpose has already been dropped from the map, so its end does
	// nothing. A one-shot slot has no queue, so its end only clears the slot.
	const FName *found = nullptr;
	for (const TPair<FName, UAudioComponent *> &Pair : audioComponents)
	{
		if (Pair.Value == component)
		{
			found = &Pair.Key;
			break;
		}
	}
	if (!found)
	{
		return;
	}
	const FName slot = *found;
	audioComponents.Remove(slot);
	TArray<FAudioInfo> *queue = audioQueues.Find(slot);
	if (!queue || queue->Num() == 0)
	{
		return;
	}
	const FAudioInfo next = (*queue)[0];
	queue->RemoveAt(0);
	UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::OnSlotAudioFinished %s: starting the next queued audio"), *slot.ToString());
	StartSlotAudio(slot, audioChannels.FindRef(slot), next);
}

void UAudioPlayer::PlayAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut, bool loop, float volume)
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
	// A fresh play takes the channel over. Its queue goes before its audio is stopped, so that
	// stopping the audio cannot start a queued track.
	audioQueues.Remove(channel);
	StopSlotAudio(channel, fadeOut);
	FAudioInfo info;
	info.Audio = audio;
	info.FadeIn = fadeIn;
	info.FadeOut = fadeOut;
	info.Volume = volume;
	info.loop = loop;
	StartSlotAudio(channel, audioChannels.FindRef(channel), info);
}

void UAudioPlayer::PauseAudio(FName channel, float fadeOut)
{
	// No channel means every channel, so that everything can be silenced at once.
	if (channel.IsNone())
	{
		for (const TPair<FName, UAudioComponent *> &Pair : audioComponents)
		{
			if (IsValid(Pair.Value) && Pair.Value->IsPlaying())
			{
				Pair.Value->SetPaused(true);
			}
		}
		return;
	}
	// Check if the audio component exists and is playing
	if (IsValid(audioComponents.FindRef(channel)) && audioComponents[channel]->IsPlaying())
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::PauseAudio %s"), *channel.ToString());
		// Only pause: fading the audio out to nothing would stop it instead, and a stopped
		// component cannot be resumed, nor could the channel's queue tell that stop from an end.
		audioComponents[channel]->SetPaused(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::PauseAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::ResumeAudio(FName channel, float fadeIn)
{
	if (channel.IsNone())
	{
		for (const TPair<FName, UAudioComponent *> &Pair : audioComponents)
		{
			if (IsValid(Pair.Value) && Pair.Value->GetPlayState() == EAudioComponentPlayState::Paused)
			{
				Pair.Value->SetPaused(false);
			}
		}
		return;
	}
	// Check if the audio component exists and is paused
	if (IsValid(audioComponents.FindRef(channel)) && audioComponents[channel]->GetPlayState() == EAudioComponentPlayState::Paused)
	{
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::ResumeAudio %s"), *channel.ToString());
		// Resume where the audio was paused. Fading it in would play it again from its start.
		audioComponents[channel]->SetPaused(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed UAudioPlayer::ResumeAudio %s"), *channel.ToString());
	}
}

void UAudioPlayer::QueueAudio(FName channel, USoundWave *audio, float fadeIn, float fadeOut, bool loop, float volume)
{
	// Check if the audio not null
	if (!audio || audio->IsPendingKill())
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::QueueAudio(%s): NULL or invalid audio"), *channel.ToString());
		return;
	}
	// Check if the audio channel exists
	if (!audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::QueueAudio(%s): channel does not exist "), *channel.ToString());
		return;
	}
	FAudioInfo info;
	info.Audio = audio;
	info.FadeIn = fadeIn;
	info.FadeOut = fadeOut;
	info.Volume = volume;
	info.loop = loop;
	UAudioComponent *current = audioComponents.FindRef(channel);
	if (!IsValid(current) || !current->IsPlaying())
	{
		// There is nothing to queue behind, so the audio starts now.
		UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::QueueAudio %s: the channel is idle, playing now"), *channel.ToString());
		StartSlotAudio(channel, audioChannels.FindRef(channel), info);
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::QueueAudio %s"), *channel.ToString());
	// Add the audio info to the queue
	audioQueues.FindOrAdd(channel).Add(info);
}

void UAudioPlayer::PlayOneShot(FName channel, USoundWave *audio, float volume)
{
	if (!audio || audio->IsPendingKill())
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayOneShot(%s): NULL or invalid audio"), *channel.ToString());
		return;
	}
	if (!audioChannels.Contains(channel))
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::PlayOneShot(%s): channel does not exist "), *channel.ToString());
		return;
	}
	FAudioInfo info;
	info.Audio = audio;
	info.Volume = volume;
	// The one-shot slot carries the channel's sound class but not the audio the channel is
	// playing, so only the one-shot before it is replaced.
	const FName slot = OneShotSlot(channel);
	StopSlotAudio(slot, 0.0f);
	StartSlotAudio(slot, audioChannels.FindRef(channel), info);
}

void UAudioPlayer::StopAudio(FName channel, float fadeOut)
{
	// Check if the audio component exists and is playing or paused
	if (IsValid(audioComponents.FindRef(channel)))
	{
		auto PlayState = audioComponents[channel]->GetPlayState();
		if (PlayState != EAudioComponentPlayState::Stopped)
		{
			UE_LOG(LogTemp, Display, TEXT("UAudioPlayer::StopAudio %s"), *channel.ToString());

			// Clear the audio queue first, so that stopping cannot start a queued track
			audioQueues.Remove(channel);

			// Fade out and stop the audio
			StopSlotAudio(channel, fadeOut);
			return;
		}
	}
	// Nothing is playing, but a queue left on the channel would start by itself later.
	audioQueues.Remove(channel);
}

TWeakObjectPtr<UAudioPlayer> UAudioPlayer::singletonInstance;

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
			if (IsValid(AudioComp) && AudioComp->IsPlaying())
			{
				// Nothing may start a queued track while the player is going away.
				AudioComp->OnAudioFinishedNative.RemoveAll(this);

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
	if (singletonInstance.Get() == this)
	{
		singletonInstance.Reset();
	}
}

UAudioPlayer *UAudioPlayer::GetSingletonInstance(UObject *Outer, bool bRooted)
{
	UWorld *World = Outer ? Outer->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UAudioPlayer::GetSingletonInstance: %s has no world."), *GetNameSafe(Outer));
		return nullptr;
	}
	// One player per world: an instance left over from another world (a previous PIE session) is not reused.
	UAudioPlayer *Instance = singletonInstance.Get();
	if (!Instance || Instance->audioWorld != World)
	{
		Instance = NewObject<UAudioPlayer>(Outer);
		Instance->audioWorld = World;
		singletonInstance = Instance;
	}
	if (bRooted && !Instance->IsRooted())
	{
		Instance->AddToRoot();
	}
	else if (!bRooted && Instance->IsRooted())
	{
		Instance->RemoveFromRoot();
	}
	return Instance;
}
