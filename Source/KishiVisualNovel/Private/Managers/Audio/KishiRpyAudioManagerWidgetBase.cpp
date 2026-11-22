#include "Managers/Audio/KishiRpyAudioManagerWidgetBase.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Player/RpyScriptInterpreter.h"
#include "Showables/LayeredSpriteHandler.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "LevelSequencePlayer.h"
#include "PaperSprite.h"
#include "TimerManager.h"
#include "Session/RpySession.h"
#include "Templates/KishiScriptInterface.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

void UKishiRpyAudioManagerWidgetBase::SetAudioManagerState_Implementation(const FRpyAudioManagerState &State)
{
	if (AudioPlayer)
	{
		for (const FRpyAudioPlayState &Current : AudioState.audioStates)
		{
			AudioPlayer->StopAudio(Current.channel, 0.0f);
		}
	}
	AudioState.audioStates.Reset();
	for (const FRpyAudioPlayState &Saved : State.audioStates)
	{
		IRpyAudioManager::Execute_PlayAudio(this, Saved.channel, Saved.rpyAudio, Saved.options);
	}
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::PauseAudio_Implementation(float Timeout)
{
	if (!AudioPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot pause audio, there is no AudioPlayer."), *GetName());
		return ERpyInstructionResult::Failed;
	}
	AudioPlayer->PauseAudio(NAME_None, Timeout);
	OnPauseAudioRequested(Timeout);
	return ERpyInstructionResult::Continue;
}

void UKishiRpyAudioManagerWidgetBase::Reset_Implementation()
{
	AudioState.audioStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::StopAudio_Implementation(FName Channel, FRpyAudioOptions Options)
{
	if (!AudioPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot stop channel '%s', there is no AudioPlayer."), *GetName(), *Channel.ToString());
		return ERpyInstructionResult::Failed;
	}
	AudioPlayer->StopAudio(Channel, Options.fadeOut);
	OnStopAudioRequested(Channel, Options);
	AudioState.audioStates.RemoveAll([Channel](const FRpyAudioPlayState &Item)
																	 { return Item.channel == Channel; });
	return ERpyInstructionResult::Continue;
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::QueueAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	USoundWave *Sound = ResolveAudio(Audio);
	if (!AudioPlayer || !Sound || !AudioPlayer->audioChannels.Contains(Channel))
	{
		LogAudioFailure(TEXT("queue"), Channel, Audio, Sound);
		return ERpyInstructionResult::Failed;
	}
	AudioPlayer->QueueAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop, Options.volume);
	OnQueueAudioRequested(Channel, Audio, Options);
	return ERpyInstructionResult::Continue;
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::PlayAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	USoundWave *Sound = ResolveAudio(Audio);
	if (!AudioPlayer || !Sound || !AudioPlayer->audioChannels.Contains(Channel))
	{
		LogAudioFailure(TEXT("play"), Channel, Audio, Sound);
		return ERpyInstructionResult::Failed;
	}
	AudioPlayer->PlayAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop, Options.volume);
	FRpyAudioPlayState *Entry = AudioState.audioStates.FindByPredicate([Channel](const FRpyAudioPlayState &Item)
																																		 { return Item.channel == Channel; });
	if (!Entry)
	{
		Entry = &AudioState.audioStates.AddDefaulted_GetRef();
	}
	Entry->channel = Channel;
	Entry->rpyAudio = Audio;
	Entry->options = Options;
	OnPlayAudioRequested(Channel, Audio, Options);
	return ERpyInstructionResult::Continue;
}

TArray<FRpyAudioManagerState> UKishiRpyAudioManagerWidgetBase::GetCurrentAudioStates_Implementation() const
{
	return {AudioState};
}

void UKishiRpyAudioManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	AudioPlayer = UAudioPlayer::GetSingletonInstance(this);
	if (AudioPlayer)
	{
		AudioPlayer->audioChannels = AudioChannels;
	}
}

void UKishiRpyAudioManagerWidgetBase::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset);
	// Play the blip directly so it is not recorded in the saved audio state, and as a one-shot
	// so that a line of dialogue does not cut off a sound effect the script started on the same
	// channel: a blip is emitted for every revealed character.
	const FName BlipChannel(TEXT("sound"));
	if (Viseme != EMouthViseme::Neutral && IsValid(Blip) && AudioPlayer && AudioPlayer->audioChannels.Contains(BlipChannel))
	{
		AudioPlayer->PlayOneShot(BlipChannel, Blip);
	}
}

void UKishiRpyAudioManagerWidgetBase::LogAudioFailure(const TCHAR *Operation, FName Channel, const FRpyAudio &Audio, const USoundWave *Sound) const
{
	if (!AudioPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio, there is no AudioPlayer."), *GetName(), Operation);
	}
	else if (!Sound)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio '%s' on channel '%s', the sound could not be loaded."), *GetName(), Operation, *Audio.path, *Channel.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot %s audio on channel '%s', add it to AudioChannels."), *GetName(), Operation, *Channel.ToString());
	}
}

USoundWave *UKishiRpyAudioManagerWidgetBase::ResolveAudio(const FRpyAudio &Audio) const
{
	if (IsValid(Audio.audio))
	{
		return Audio.audio;
	}
	return Audio.path.IsEmpty() ? nullptr : LoadObject<USoundWave>(nullptr, *Audio.path);
}
