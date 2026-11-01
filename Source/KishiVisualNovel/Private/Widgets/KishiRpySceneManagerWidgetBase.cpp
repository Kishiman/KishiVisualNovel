#include "Widgets/KishiRpySceneManagerWidgetBase.h"
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

void UKishiRpySceneManagerWidgetBase::SetSceneManagerState_Implementation(const FRpySceneManagerState &State)
{
	IRpySceneManager::Execute_Scene(this, State.rpyImage, State.options);
}

bool UKishiRpySceneManagerWidgetBase::Scene_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	SceneState.rpyImage = Image;
	SceneState.options = Options;
	if (Image.type == ERpyImageType::ENONE)
	{
		// "scene _" clears the scene.
		if (IsValid(SceneSprite))
			SceneSprite->SetVisibility(ESlateVisibility::Collapsed);
		OnSceneRequested(Image, Options);
		return true;
	}
	if (!IsValid(SceneSprite) && SceneSpriteClass)
	{
		SceneSprite = CreateWidget<UUserWidget>(this, SceneSpriteClass);
		if (SceneSprite)
		{
			bOwnsSceneSprite = true;
			// Below the player widget, so the background never covers its sprites and dialogue.
			SceneSprite->AddToViewport(-100);
			UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: no SceneSprite widget in the tree, added %s to the viewport behind the player."), *GetName(), *SceneSprite->GetName());
		}
	}
	bool bHandled = false;
	if (IsValid(SceneSprite))
		SceneSprite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
			if (IsValid(ActiveLevelSequencePlayer))
				ActiveLevelSequencePlayer->Stop();
			FMovieSceneSequencePlaybackSettings Settings;
			Settings.bAutoPlay = true;
			ALevelSequenceActor *SequenceActor = nullptr;
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
	if (!bHandled)
	{
		// Log and continue so a missing asset or SceneSprite never stalls the script.
		UE_LOG(LogTemp, Error, TEXT("%s: could not display scene image '%s' of type %s. Check the image's asset, and add a SceneSprite widget or set SceneSpriteClass."),
					 *GetName(), *Image.tag.ToString(), *UEnum::GetValueAsString(Image.type));
	}
	OnSceneRequested(Image, Options);
	return true;
}

void UKishiRpySceneManagerWidgetBase::Reset_Implementation()
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
