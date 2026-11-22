#include "Managers/Audio/RpyAudioManager.h"

#include "Managers/Audio/AudioPlayer.h"
#include "Sound/SoundWave.h"

namespace
{
	FString GetManagerName(const TScriptInterface<IRpyAudioManager> &Target)
	{
		return GetNameSafe(Target.GetObject());
	}

	/** Returns the sound to play, or null after logging why the operation cannot run. */
	USoundWave *ResolvePlayableAudio(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, const TCHAR *Operation, FName Channel, const FRpyAudio &Audio)
	{
		if (!IsValid(Player))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio, there is no AudioPlayer."), *GetManagerName(Target), Operation);
			return nullptr;
		}
		USoundWave *Sound = URpyAudioManagerLibrary::ResolveAudio(Audio);
		if (!Sound)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio '%s' on channel '%s', the sound could not be loaded."), *GetManagerName(Target), Operation, *Audio.path,
				   *Channel.ToString());
			return nullptr;
		}
		if (!Player->audioChannels.Contains(Channel))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio on channel '%s', add it to AudioChannels."), *GetManagerName(Target), Operation, *Channel.ToString());
			return nullptr;
		}
		return Sound;
	}
}

USoundWave *URpyAudioManagerLibrary::ResolveAudio(const FRpyAudio &Audio)
{
	if (IsValid(Audio.audio))
	{
		return Audio.audio;
	}
	return Audio.path.IsEmpty() ? nullptr : LoadObject<USoundWave>(nullptr, *Audio.path);
}

ERpyInstructionResult URpyAudioManagerLibrary::PlayAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudio &Audio,
																 const FRpyAudioOptions &Options)
{
	USoundWave *Sound = ResolvePlayableAudio(Target, Player, TEXT("play"), Channel, Audio);
	if (!Sound)
	{
		return ERpyInstructionResult::Failed;
	}
	Player->PlayAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop, Options.volume);
	return ERpyInstructionResult::Continue;
}

ERpyInstructionResult URpyAudioManagerLibrary::QueueAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudio &Audio,
																  const FRpyAudioOptions &Options)
{
	USoundWave *Sound = ResolvePlayableAudio(Target, Player, TEXT("queue"), Channel, Audio);
	if (!Sound)
	{
		return ERpyInstructionResult::Failed;
	}
	Player->QueueAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop, Options.volume);
	return ERpyInstructionResult::Continue;
}

ERpyInstructionResult URpyAudioManagerLibrary::StopAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, FName Channel, const FRpyAudioOptions &Options)
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot stop channel '%s', there is no AudioPlayer."), *GetManagerName(Target), *Channel.ToString());
		return ERpyInstructionResult::Failed;
	}
	Player->StopAudio(Channel, Options.fadeOut);
	return ERpyInstructionResult::Continue;
}

ERpyInstructionResult URpyAudioManagerLibrary::PauseAudio_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, float Timeout)
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot pause audio, there is no AudioPlayer."), *GetManagerName(Target));
		return ERpyInstructionResult::Failed;
	}
	Player->PauseAudio(NAME_None, Timeout);
	return ERpyInstructionResult::Continue;
}

void URpyAudioManagerLibrary::SetAudioManagerState_Default(const TScriptInterface<IRpyAudioManager> &Target, UAudioPlayer *Player, const FRpyAudioManagerState &PreviousState,
														   const FRpyAudioManagerState &State)
{
	if (IsValid(Player))
	{
		for (const FRpyAudioPlayState &Current : PreviousState.audioStates)
		{
			Player->StopAudio(Current.channel, 0.0f);
		}
	}
	UObject *Manager = Target.GetObject();
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Error, TEXT("SetAudioManagerState_Default requires a valid manager."));
		return;
	}
	for (const FRpyAudioPlayState &Saved : State.audioStates)
	{
		IRpyAudioManager::Execute_PlayAudio(Manager, Saved.channel, Saved.rpyAudio, Saved.options);
	}
}

void URpyAudioManagerLibrary::PlayDialogueBlip(UAudioPlayer *Player, USoundWave *Blip, EMouthViseme Viseme)
{
	static const FName BlipChannel(TEXT("sound"));
	if (Viseme != EMouthViseme::Neutral && IsValid(Blip) && IsValid(Player) && Player->audioChannels.Contains(BlipChannel))
	{
		Player->PlayOneShot(BlipChannel, Blip);
	}
}

void URpyAudioManagerLibrary::RecordPlayedAudio(FRpyAudioManagerState &State, FName Channel, const FRpyAudio &Audio, const FRpyAudioOptions &Options)
{
	FRpyAudioPlayState *Entry = State.audioStates.FindByPredicate([Channel](const FRpyAudioPlayState &Item)
																  { return Item.channel == Channel; });
	if (!Entry)
	{
		Entry = &State.audioStates.AddDefaulted_GetRef();
	}
	Entry->channel = Channel;
	Entry->rpyAudio = Audio;
	Entry->options = Options;
}

void URpyAudioManagerLibrary::RecordStoppedAudio(FRpyAudioManagerState &State, FName Channel)
{
	State.audioStates.RemoveAll([Channel](const FRpyAudioPlayState &Item)
								{ return Item.channel == Channel; });
}
