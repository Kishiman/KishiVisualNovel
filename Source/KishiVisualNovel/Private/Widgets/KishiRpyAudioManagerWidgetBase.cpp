#include "Widgets/KishiRpyAudioManagerWidgetBase.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
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
#include "Rpy/RpySession.h"
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

bool UKishiRpyAudioManagerWidgetBase::PauseAudio_Implementation(float Timeout)
{
	if (!AudioPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot pause audio, there is no AudioPlayer."), *GetName());
		return true;
	}
	AudioPlayer->PauseAudio(NAME_None, Timeout);
	OnPauseAudioRequested(Timeout);
	return true;
}

void UKishiRpyAudioManagerWidgetBase::Reset_Implementation()
{
	AudioState.audioStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

bool UKishiRpyAudioManagerWidgetBase::StopAudio_Implementation(FName Channel, FRpyAudioOptions Options)
{
	if (!AudioPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot stop channel '%s', there is no AudioPlayer."), *GetName(), *Channel.ToString());
		return true;
	}
	AudioPlayer->StopAudio(Channel, Options.fadeOut);
	OnStopAudioRequested(Channel, Options);
	AudioState.audioStates.RemoveAll([Channel](const FRpyAudioPlayState &Item)
																	 { return Item.channel == Channel; });
	return true;
}

bool UKishiRpyAudioManagerWidgetBase::QueueAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	USoundWave *Sound = ResolveAudio(Audio);
	if (!AudioPlayer || !Sound || !AudioPlayer->audioChannels.Contains(Channel))
	{
		LogAudioFailure(TEXT("queue"), Channel, Audio, Sound);
		return true;
	}
	AudioPlayer->QueueAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop);
	OnQueueAudioRequested(Channel, Audio, Options);
	return true;
}

bool UKishiRpyAudioManagerWidgetBase::PlayAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	USoundWave *Sound = ResolveAudio(Audio);
	if (!AudioPlayer || !Sound || !AudioPlayer->audioChannels.Contains(Channel))
	{
		LogAudioFailure(TEXT("play"), Channel, Audio, Sound);
		return true;
	}
	AudioPlayer->PlayAudio(Channel, Sound, Options.fadeIn, Options.fadeOut, Options.loop);
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
	return true;
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
	// Play the blip directly so it is not recorded in the saved audio state.
	const FName BlipChannel(TEXT("sound"));
	if (Viseme != EMouthViseme::Neutral && IsValid(Blip) && AudioPlayer && AudioPlayer->audioChannels.Contains(BlipChannel))
	{
		AudioPlayer->PlayAudio(BlipChannel, Blip, 0.0f, 0.0f, false);
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
