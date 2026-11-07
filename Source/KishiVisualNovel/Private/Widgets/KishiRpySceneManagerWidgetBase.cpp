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
#include "LevelSequenceActor.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "EngineUtils.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/RpyCameraAnchor.h"
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
	// Anchor and camera transition first: "scene _" with a transition still moves the camera.
	ApplySceneCameraTransition(Options);
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
		// A level sequence image replaces any transition sequence started above.
		if (IsValid(Image.levelSequence))
			bHandled = PlaySceneSequence(Image.levelSequence, FindSceneCamera());
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

AActor *UKishiRpySceneManagerWidgetBase::FindSceneCamera_Implementation() const
{
	APlayerController *PlayerController = GetOwningPlayer();
	if (!PlayerController)
		PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
		return nullptr;
	if (PlayerController->PlayerCameraManager)
		if (AActor *ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget())
			return ViewTarget;
	return PlayerController->GetViewTarget();
}

USceneComponent *UKishiRpySceneManagerWidgetBase::FindSceneAnchor_Implementation(FName Layer) const
{
	static const FName MasterLayer(TEXT("master"));
	UWorld *World = GetWorld();
	if (Layer.IsNone() || Layer == MasterLayer || !World)
		return nullptr;
	const TScriptInterface<IRpyCameraAnchor> CameraAnchor = URpyCameraAnchorLibrary::FindAnchorByLayerName(World, Layer);
	if (UObject *AnchorObject = CameraAnchor.GetObject())
	{
		if (USceneComponent *AnchorComponent = Cast<USceneComponent>(AnchorObject))
			return AnchorComponent;
		if (AActor *AnchorActor = IRpyCameraAnchor::Execute_GetActor(AnchorObject))
			if (USceneComponent *AnchorRoot = AnchorActor->GetRootComponent())
				return AnchorRoot;
	}
	UClass *Class = AnchorClass ? AnchorClass.Get() : AActor::StaticClass();
	for (TActorIterator<AActor> It(World, Class); It; ++It)
		if (It->ActorHasTag(Layer) && It->GetRootComponent())
			return It->GetRootComponent();
	UE_LOG(LogTemp, Warning, TEXT("%s: no camera anchor or %s actor tagged with scene layer '%s'; the camera stays on its current parent."), *GetName(),
				 *Class->GetName(), *Layer.ToString());
	return nullptr;
}

bool UKishiRpySceneManagerWidgetBase::AttachCameraToAnchor(AActor *Camera, USceneComponent *Anchor)
{
	if (!IsValid(Camera) || !IsValid(Anchor) || Anchor->GetOwner() == Camera)
		return false;
	USceneComponent *CameraRoot = Camera->GetRootComponent();
	if (!CameraRoot)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot attach camera %s to anchor %s because the camera has no root component."), *GetName(), *Camera->GetName(), *Anchor->GetName());
		return false;
	}
	if (AnchoredCamera != Camera)
	{
		// A different camera: put the previous one back, then remember where this one came from.
		RestoreSceneCamera();
		AnchoredCamera = Camera;
		CameraOriginalParent = CameraRoot->GetAttachParent();
		CameraOriginalSocket = CameraRoot->GetAttachSocketName();
		CameraOriginalRelativeTransform = CameraRoot->GetRelativeTransform();
	}
	if (CameraRoot->Mobility != EComponentMobility::Movable)
		CameraRoot->SetMobility(EComponentMobility::Movable);
	const bool bAttached = CameraRoot->AttachToComponent(Anchor, FAttachmentTransformRules::KeepRelativeTransform);
	VNSCENE_LOG("attached camera %s to anchor %s.%s: %d", *Camera->GetName(), *GetNameSafe(Anchor->GetOwner()), *Anchor->GetName(), bAttached ? 1 : 0);
	return bAttached;
}

bool UKishiRpySceneManagerWidgetBase::PlaySceneSequence(ULevelSequence *Sequence, AActor *Camera)
{
	StopSceneSequence();
	if (!IsValid(Sequence))
		return false;
	FMovieSceneSequencePlaybackSettings Settings;
	// No autoplay: the camera binding must be set before the first evaluation.
	Settings.bAutoPlay = false;
	ALevelSequenceActor *SequenceActor = nullptr;
	ULevelSequencePlayer *Player = ULevelSequencePlayer::CreateLevelSequencePlayer(this, Sequence, Settings, SequenceActor);
	if (!Player || !SequenceActor)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create a player for level sequence %s."), *GetName(), *Sequence->GetName());
		return false;
	}
	ActiveLevelSequencePlayer = Player;
	ActiveLevelSequenceActor = SequenceActor;
	UMovieScene *MovieScene = Sequence->GetMovieScene();
	if (IsValid(Camera) && MovieScene)
	{
		if (!CameraBindingTag.IsNone() && MovieScene->AllTaggedBindings().Contains(CameraBindingTag))
		{
			SequenceActor->SetBindingByTag(CameraBindingTag, {Camera});
			VNSCENE_LOG("bound sequence %s tag '%s' to camera %s", *Sequence->GetName(), *CameraBindingTag.ToString(), *Camera->GetName());
		}
		else
		{
			const FGuid CameraGuid = FindSequenceCameraBinding(MovieScene);
			if (CameraGuid.IsValid())
			{
				SequenceActor->SetBinding(UE::MovieScene::FFixedObjectBindingID(CameraGuid, MovieSceneSequenceID::Root), {Camera});
				VNSCENE_LOG("bound sequence %s camera binding '%s' to camera %s (no binding tagged '%s')", *Sequence->GetName(),
										*MovieScene->GetObjectDisplayName(CameraGuid).ToString(), *Camera->GetName(), *CameraBindingTag.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: level sequence %s has no binding tagged '%s', no camera cut binding and more than one possessable; it plays with the bindings saved in the asset, so the scene camera does not move."),
							 *GetName(), *Sequence->GetName(), *CameraBindingTag.ToString());
			}
		}
	}
	Player->Play();
	return true;
}

FGuid UKishiRpySceneManagerWidgetBase::FindSequenceCameraBinding(UMovieScene *MovieScene)
{
	if (!MovieScene)
		return FGuid();
	// The camera a camera cut cuts to is the sequence's camera.
	if (const UMovieSceneTrack *CameraCutTrack = MovieScene->GetCameraCutTrack())
		for (const UMovieSceneSection *Section : CameraCutTrack->GetAllSections())
			if (const UMovieSceneCameraCutSection *CutSection = Cast<UMovieSceneCameraCutSection>(Section))
			{
				const FGuid Guid = CutSection->GetCameraBindingID().GetGuid();
				if (Guid.IsValid() && MovieScene->FindPossessable(Guid))
					return Guid;
			}
	// Otherwise a sequence that possesses a single object animates the camera.
	if (MovieScene->GetPossessableCount() == 1)
		return MovieScene->GetPossessable(0).GetGuid();
	return FGuid();
}

void UKishiRpySceneManagerWidgetBase::StopSceneSequence()
{
	if (IsValid(ActiveLevelSequencePlayer))
		ActiveLevelSequencePlayer->Stop();
	if (IsValid(ActiveLevelSequenceActor))
		ActiveLevelSequenceActor->Destroy();
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
	ULevelSequence *Sequence = nullptr;
	if (Options.with == ERPYTransitionType::LEVEL_SEQUENCE || Options.transition.type == ERPYTransitionType::LEVEL_SEQUENCE)
	{
		Sequence = Options.transition.levelSequence;
		if (!IsValid(Sequence) && !Options.transition.path.IsEmpty())
			Sequence = LoadObject<ULevelSequence>(nullptr, *Options.transition.path);
		if (!IsValid(Sequence))
			UE_LOG(LogTemp, Error, TEXT("%s: scene transition '%s' has no level sequence (path '%s')."), *GetName(), *Options.transition.name.ToString(),
						 *Options.transition.path);
	}
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
