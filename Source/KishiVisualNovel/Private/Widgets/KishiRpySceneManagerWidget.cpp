#include "Widgets/KishiRpySceneManagerWidget.h"
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
#include "KishiRpyWidgetReflection.h"

void UKishiRpySceneManagerWidget::SetSceneManagerState_Implementation(const FRpySceneManagerState& State)
{
	SceneState = State;
	Scene_Implementation(State.rpyImage, State.options);
}

bool UKishiRpySceneManagerWidget::Scene_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	SceneState.rpyImage = Image;
	SceneState.options = Options;
	if (!IsValid(SceneSprite) && SceneSpriteClass)
	{
		SceneSprite = CreateWidget<UUserWidget>(this, SceneSpriteClass);
		if (SceneSprite)
		{
			bOwnsSceneSprite = true;
			SceneSprite->AddToViewport();
		}
	}
	bool bHandled = false;
	if (IsValid(SceneSprite)) SceneSprite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	switch (Image.type)
	{
	case ERpyImageType::ESPRITE:
		if (IsValid(SceneSprite) && IsValid(Image.sprite))
		{
			SetImageOnWidget(SceneSprite, Image.sprite, SceneTargetSize);
			bHandled = true;
		}
		break;
	case ERpyImageType::EVIDEO_MEDIA_SPRITE:
		if (IsValid(SceneSprite) && IsValid(Image.videoMediaSprite) && IsValid(Image.videoMediaSprite->fileMediaSource))
		{
			SetMediaOnWidget(SceneSprite, Image.videoMediaSprite->fileMediaSource, SceneTargetSize);
			bHandled = true;
		}
		break;
	case ERpyImageType::ELEVEL_SEQUENCE:
		if (IsValid(Image.levelSequence))
		{
			if (IsValid(ActiveLevelSequencePlayer)) ActiveLevelSequencePlayer->Stop();
			FMovieSceneSequencePlaybackSettings Settings;
			Settings.bAutoPlay = true;
			ALevelSequenceActor* SequenceActor = nullptr;
			ActiveLevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, Image.levelSequence, Settings, SequenceActor);
			if (ActiveLevelSequencePlayer)
			{
				ActiveLevelSequencePlayer->Play();
				bHandled = true;
			}
		}
		break;
	default:
		break;
	}
	OnSceneRequested(Image, Options);
	return bHandled;
}

void UKishiRpySceneManagerWidget::Reset_Implementation()
{
	if (IsValid(ActiveLevelSequencePlayer))
	{
		ActiveLevelSequencePlayer->Stop();
		ActiveLevelSequencePlayer = nullptr;
	}
	if (IsValid(SceneSprite))
	{
		if (bOwnsSceneSprite)
		{
			SceneSprite->RemoveFromParent();
			SceneSprite = nullptr;
		}
		else
		{
			SceneSprite->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	bOwnsSceneSprite = false;
	SceneState = FRpySceneManagerState();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
