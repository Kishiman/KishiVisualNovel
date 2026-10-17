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
		PlayAudio_Implementation(Saved.channel, Saved.rpyAudio, Saved.options);
	}
}

bool UKishiRpyAudioManagerWidgetBase::PauseAudio_Implementation(float Timeout)
{
	if (!AudioPlayer)
	{
		return false;
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
		return false;
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
		return false;
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
		return false;
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
	if (Viseme != EMouthViseme::Neutral && IsValid(Blip))
	{
		FRpyAudioOptions Options;
		FRpyAudio Audio;
		Audio.audio = Blip;
		PlayAudio_Implementation(FName(TEXT("sound")), Audio, Options);
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
