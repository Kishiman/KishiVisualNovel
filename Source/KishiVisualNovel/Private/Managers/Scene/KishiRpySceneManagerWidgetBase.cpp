#include "Managers/Scene/KishiRpySceneManagerWidgetBase.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Player/RpyScriptInterpreter.h"
#include "Showables/LayeredSpriteHandler.h"
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
#include "LevelSequenceActor.h"
#include "LevelSequence.h"
#include "Showables/VideoMediaSprite.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "EngineUtils.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "World/RpyCameraAnchor.h"
#include "PaperSprite.h"
#include "TimerManager.h"
#include "Session/RpySession.h"
#include "Templates/KishiScriptInterface.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "Player/KishiRpyWidgetReflection.h"
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
	URpySceneManagerLibrary::SetSceneManagerState_Default(this, State);
}

ERpyInstructionResult UKishiRpySceneManagerWidgetBase::Scene_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	VNSCENE_LOG("scene tag='%s' asset=%s", *Image.tag.ToString(), *Image.asset.ToString());
	SceneState.rpyImage = Image;
	SceneState.options = Options;
	// Anchor and camera transition first: "scene _" with a transition still moves the camera.
	ApplySceneCameraTransition(Options);
	if (Image.asset.IsNull())
	{
		// "scene _" clears the scene.
		if (IsValid(SceneSprite))
			SceneSprite->SetVisibility(ESlateVisibility::Collapsed);
		OnSceneRequested(Image, Options);
		return ERpyInstructionResult::Continue;
	}
	UObject *Asset = Image.LoadAsset();
	EnsureSceneSprite();
	bool bHandled = false;
	if (IsValid(SceneSprite))
		SceneSprite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (ULevelSequence *Sequence = Cast<ULevelSequence>(Asset))
	{
		// A level sequence image replaces any transition sequence started above.
		bHandled = PlaySceneSequence(Sequence, FindSceneCamera());
	}
	else if (UPaperSprite *PaperSprite = Cast<UPaperSprite>(Asset))
	{
		if (IsValid(SceneSprite))
		{
			bHandled = SetImageOnWidget(SceneSprite, PaperSprite, SceneTargetSize);
			if (!bHandled)
				UE_LOG(LogTemp, Error, TEXT("%s: SceneSprite %s has no SetSprite function."), *GetName(), *SceneSprite->GetClass()->GetName());
		}
	}
	else if (UVideoMediaSprite *Video = Cast<UVideoMediaSprite>(Asset))
	{
		if (IsValid(SceneSprite) && IsValid(Video->fileMediaSource))
		{
			bHandled = SetMediaOnWidget(SceneSprite, Video->fileMediaSource, SceneTargetSize);
			if (!bHandled)
				UE_LOG(LogTemp, Error, TEXT("%s: SceneSprite %s has no SetFileMediaSource function."), *GetName(), *SceneSprite->GetClass()->GetName());
		}
	}
	if (!bHandled)
	{
		// Reported as Failed below; the session applies the Failed Instruction Policy.
		UE_LOG(LogTemp, Error, TEXT("%s: could not display scene image '%s' (%s). A scene shows a Paper2D sprite or a video media sprite in SceneSprite (add one or set SceneSpriteClass), or plays a level sequence."),
					 *GetName(), *Image.tag.ToString(), *Image.asset.ToString());
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
	return bHandled ? ERpyInstructionResult::Continue : ERpyInstructionResult::Failed;
}

AActor *UKishiRpySceneManagerWidgetBase::FindSceneCamera_Implementation() const
{
	return URpySceneManagerLibrary::FindSceneCamera(this, GetOwningPlayer());
}

USceneComponent *UKishiRpySceneManagerWidgetBase::FindSceneAnchor_Implementation(FName Layer) const
{
	return URpySceneManagerLibrary::FindSceneAnchor(this, Layer, AnchorClass);
}

bool UKishiRpySceneManagerWidgetBase::AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor)
{
	if (!IsValid(Camera) || !IsValid(Anchor) || Anchor->GetOwner() == Camera || !Camera->GetRootComponent())
		return URpySceneManagerLibrary::AttachCameraToAnchor(Camera, Anchor);
	if (AnchoredCamera != Camera)
	{
		// A different camera: put the previous one back, then remember where this one came from.
		RestoreSceneCamera();
		USceneComponent *CameraRoot = Camera->GetRootComponent();
		AnchoredCamera = Camera;
		CameraOriginalParent = CameraRoot->GetAttachParent();
		CameraOriginalSocket = CameraRoot->GetAttachSocketName();
		CameraOriginalRelativeTransform = CameraRoot->GetRelativeTransform();
	}
	return URpySceneManagerLibrary::AttachCameraToAnchor(Camera, Anchor);
}

bool UKishiRpySceneManagerWidgetBase::PlaySceneSequence(ULevelSequence *Sequence, AActor *Camera)
{
	StopSceneSequence();
	return URpySceneManagerLibrary::PlayCameraSequence(this, Sequence, Camera, CameraBindingTag, ActiveLevelSequencePlayer, ActiveLevelSequenceActor);
}

void UKishiRpySceneManagerWidgetBase::StopSceneSequence()
{
	URpySceneManagerLibrary::StopCameraSequence(ActiveLevelSequencePlayer, ActiveLevelSequenceActor);
	ActiveLevelSequencePlayer = nullptr;
	ActiveLevelSequenceActor = nullptr;
}

void UKishiRpySceneManagerWidgetBase::RestoreSceneCamera()
{
	if (IsValid(AnchoredCamera))
		if (USceneComponent *CameraRoot = AnchoredCamera->GetRootComponent())
		{
			if (IsValid(CameraOriginalParent))
				CameraRoot->AttachToComponent(CameraOriginalParent, FAttachmentTransformRules::KeepRelativeTransform, CameraOriginalSocket);
			else
				CameraRoot->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			CameraRoot->SetRelativeTransform(CameraOriginalRelativeTransform);
		}
	AnchoredCamera = nullptr;
	CameraOriginalParent = nullptr;
	CameraOriginalSocket = NAME_None;
}

bool UKishiRpySceneManagerWidgetBase::ApplySceneCameraTransition(const FRpySceneOptions &Options)
{
	ULevelSequence *Sequence = URpySceneManagerLibrary::GetTransitionSequence(Options);
	USceneComponent *Anchor = FindSceneAnchor(Options.layer);
	if (!Anchor && !Sequence)
		return false;
	// Stop the previous sequence before reading the view target, so its camera cuts do not decide which camera moves.
	if (Sequence)
		StopSceneSequence();
	AActor *Camera = FindSceneCamera();
	if (!IsValid(Camera))
		UE_LOG(LogTemp, Error, TEXT("%s: no active camera for scene layer '%s' / transition '%s'."), *GetName(), *Options.layer.ToString(),
					 *Options.transition.name.ToString());
	if (Anchor)
		AttachCameraToAnchor(Camera, Anchor);
	return Sequence && PlaySceneSequence(Sequence, Camera);
}

void UKishiRpySceneManagerWidgetBase::Reset_Implementation()
{
	StopSceneSequence();
	if (bRestoreCameraOnReset)
		RestoreSceneCamera();
	else
	{
		AnchoredCamera = nullptr;
		CameraOriginalParent = nullptr;
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
