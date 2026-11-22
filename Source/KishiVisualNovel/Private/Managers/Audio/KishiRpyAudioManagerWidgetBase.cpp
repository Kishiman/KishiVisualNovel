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
	const FRpyAudioManagerState Previous = AudioState;
	const FRpyAudioManagerState Saved = State;
	AudioState.audioStates.Reset();
	URpyAudioManagerLibrary::SetAudioManagerState_Default(this, AudioPlayer, Previous, Saved);
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::PauseAudio_Implementation(float Timeout)
{
	const ERpyInstructionResult Result = URpyAudioManagerLibrary::PauseAudio_Default(this, AudioPlayer, Timeout);
	if (Result != ERpyInstructionResult::Failed)
	{
		OnPauseAudioRequested(Timeout);
	}
	return Result;
}

void UKishiRpyAudioManagerWidgetBase::Reset_Implementation()
{
	AudioState.audioStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::StopAudio_Implementation(FName Channel, FRpyAudioOptions Options)
{
	const ERpyInstructionResult Result = URpyAudioManagerLibrary::StopAudio_Default(this, AudioPlayer, Channel, Options);
	if (Result != ERpyInstructionResult::Failed)
	{
		OnStopAudioRequested(Channel, Options);
		URpyAudioManagerLibrary::RecordStoppedAudio(AudioState, Channel);
	}
	return Result;
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::QueueAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	const ERpyInstructionResult Result = URpyAudioManagerLibrary::QueueAudio_Default(this, AudioPlayer, Channel, Audio, Options);
	if (Result != ERpyInstructionResult::Failed)
	{
		OnQueueAudioRequested(Channel, Audio, Options);
	}
	return Result;
}

ERpyInstructionResult UKishiRpyAudioManagerWidgetBase::PlayAudio_Implementation(FName Channel, FRpyAudio Audio, FRpyAudioOptions Options)
{
	const ERpyInstructionResult Result = URpyAudioManagerLibrary::PlayAudio_Default(this, AudioPlayer, Channel, Audio, Options);
	if (Result != ERpyInstructionResult::Failed)
	{
		URpyAudioManagerLibrary::RecordPlayedAudio(AudioState, Channel, Audio, Options);
		OnPlayAudioRequested(Channel, Audio, Options);
	}
	return Result;
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
	URpyAudioManagerLibrary::PlayDialogueBlip(AudioPlayer, Blip, Viseme);
}

