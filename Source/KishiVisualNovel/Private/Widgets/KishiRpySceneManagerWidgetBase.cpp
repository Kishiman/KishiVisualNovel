#include "Widgets/KishiRpySceneManagerWidgetBase.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Components/Button.h"
#include "Components/Image.h"
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
#include "Blueprint/WidgetTree.h"

// Temporary scene tracing. Filter the Output Log on "[VNScene]".
#define VNSCENE_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNScene] %s: " Format), *GetName(), ##__VA_ARGS__)

TSharedRef<SWidget> UKishiRpySceneManagerWidgetBase::RebuildWidget()
{
	// Before the Slate widget is built, so a SceneSprite made the root of an empty tree is part of it.
	if (!IsDesignTime())
		EnsureSceneSprite();
	return Super::RebuildWidget();
}

void UKishiRpySceneManagerWidgetBase::EnsureSceneSprite()
{
	if (IsValid(SceneSprite) || !SceneSpriteClass || !WidgetTree)
		return;
	UUserWidget *Sprite = WidgetTree->ConstructWidget<UUserWidget>(SceneSpriteClass, TEXT("SceneSprite"));
	if (!Sprite)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create SceneSpriteClass %s."), *GetName(), *GetNameSafe(SceneSpriteClass));
		return;
	}
	SceneSprite = Sprite;
	if (!WidgetTree->RootWidget)
	{
		WidgetTree->RootWidget = SceneSprite;
		VNSCENE_LOG("created %s (%s) as the root of the empty widget tree.", *SceneSprite->GetName(), *SceneSpriteClass->GetName());
	}
	else if (UPanelWidget *RootPanel = Cast<UPanelWidget>(WidgetTree->RootWidget))
	{
		// First child, so it draws behind anything else the manager's tree holds.
		if (UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(RootPanel->InsertChildAt(0, SceneSprite)))
		{
			CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			CanvasSlot->SetOffsets(FMargin(0.0f));
		}
		VNSCENE_LOG("created %s (%s) behind the children of root panel %s.", *SceneSprite->GetName(), *SceneSpriteClass->GetName(), *RootPanel->GetName());
	}
	else
	{
		bOwnsSceneSprite = true;
		// Below the player widget, so the background never covers its sprites and dialogue.
		SceneSprite->AddToViewport(-100);
		VNSCENE_LOG("root %s is not a panel, added %s (%s) to the viewport behind the player.", *WidgetTree->RootWidget->GetName(), *SceneSprite->GetName(),
								*SceneSpriteClass->GetName());
	}
}

void UKishiRpySceneManagerWidgetBase::SetSceneManagerState_Implementation(const FRpySceneManagerState &State)
{
	IRpySceneManager::Execute_Scene(this, State.rpyImage, State.options);
}

bool UKishiRpySceneManagerWidgetBase::Scene_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	VNSCENE_LOG("scene tag='%s' type=%s sprite=%s video=%s sequence=%s", *Image.tag.ToString(), *UEnum::GetValueAsString(Image.type), *GetNameSafe(Image.sprite),
							*GetNameSafe(Image.videoMediaSprite), *GetNameSafe(Image.levelSequence));
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
	EnsureSceneSprite();
	bool bHandled = false;
	if (IsValid(SceneSprite))
		SceneSprite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	switch (Image.type)
	{
	case ERpyImageType::ESPRITE:
		if (IsValid(SceneSprite) && IsValid(Image.sprite))
		{
			bHandled = SetImageOnWidget(SceneSprite, Image.sprite, SceneTargetSize);
			if (!bHandled)
				UE_LOG(LogTemp, Error, TEXT("%s: SceneSprite %s has no SetSprite function."), *GetName(), *SceneSprite->GetClass()->GetName());
		}
		break;
	case ERpyImageType::EVIDEO_MEDIA_SPRITE:
		if (IsValid(SceneSprite) && IsValid(Image.videoMediaSprite) && IsValid(Image.videoMediaSprite->fileMediaSource))
		{
			bHandled = SetMediaOnWidget(SceneSprite, Image.videoMediaSprite->fileMediaSource, SceneTargetSize);
			if (!bHandled)
				UE_LOG(LogTemp, Error, TEXT("%s: SceneSprite %s has no SetFileMediaSource function."), *GetName(), *SceneSprite->GetClass()->GetName());
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
	FString SpriteBrush = TEXT("None");
	if (IsValid(SceneSprite))
		if (FObjectProperty *ImageProperty = FindFProperty<FObjectProperty>(SceneSprite->GetClass(), TEXT("SpriteImage")))
			if (UImage *SpriteImage = Cast<UImage>(ImageProperty->GetObjectPropertyValue_InContainer(SceneSprite)))
				SpriteBrush = FString::Printf(TEXT("%s size=%s opacity=%.2f"), *GetNameSafe(SpriteImage->Brush.GetResourceObject()), *SpriteImage->Brush.ImageSize.ToString(),
																			SpriteImage->ColorAndOpacity.A);
	VNSCENE_LOG("sceneSprite class=%s brush=%s", IsValid(SceneSprite) ? *SceneSprite->GetClass()->GetName() : TEXT("None"), *SpriteBrush);
	VNSCENE_LOG("handled=%d sceneSprite=%s parent=%s inViewport=%d visibility=%s managerVisibility=%s managerSize=%s", bHandled ? 1 : 0, *GetNameSafe(SceneSprite),
							IsValid(SceneSprite) ? *GetNameSafe(SceneSprite->GetParent()) : TEXT("None"), IsValid(SceneSprite) && SceneSprite->IsInViewport() ? 1 : 0,
							IsValid(SceneSprite) ? *UEnum::GetValueAsString(SceneSprite->GetVisibility()) : TEXT("None"), *UEnum::GetValueAsString(GetVisibility()),
							*GetCachedGeometry().GetLocalSize().ToString());
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
