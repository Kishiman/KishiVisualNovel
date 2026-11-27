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
#include "Blueprint/WidgetTree.h"

// Temporary scene tracing. Filter the Output Log on "[VNScene]".
#define VNSCENE_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNScene] %s: " Format), *GetName(), ##__VA_ARGS__)

TSharedRef<SWidget> UKishiRpySceneManagerWidgetBase::RebuildWidget()
{
	// Before the Slate widget is built, so a SceneCanvas made here is part of it.
	if (!IsDesignTime())
		EnsureSceneCanvas();
	return Super::RebuildWidget();
}

void UKishiRpySceneManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsDesignTime())
		return;
	EnsureSceneCanvas();
	GetLayer();
	if (IsValid(SceneSprite))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: SceneSprite is no longer used; backgrounds are shown on SceneCanvas. It is collapsed; delete it from the Widget Blueprint."), *GetName());
		SceneSprite->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKishiRpySceneManagerWidgetBase::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (ShowableLayer)
		ShowableLayer->Tick();
}

UKishiShowableLayer *UKishiRpySceneManagerWidgetBase::GetLayer()
{
	if (!ShowableLayer)
	{
		ShowableLayer = NewObject<UKishiShowableLayer>(this, TEXT("SceneLayer"));
		ShowableLayer->Initialize(this, SceneCanvas);
	}
	ShowableLayer->SetCanvas(SceneCanvas);
	ShowableLayer->Placement = EKishiShowablePlacement::Fill;
	ShowableLayer->TargetSize = SceneTargetSize;
	return ShowableLayer;
}

void UKishiRpySceneManagerWidgetBase::EnsureSceneCanvas()
{
	if (IsValid(SceneCanvas) || !WidgetTree)
		return;
	UWidget *OldRoot = WidgetTree->RootWidget;
	UCanvasPanel *Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SceneRoot"));
	SceneCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SceneCanvas"));
	if (!Root || !SceneCanvas)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create SceneCanvas."), *GetName());
		SceneCanvas = nullptr;
		return;
	}
	auto FillCanvas = [](UCanvasPanelSlot *CanvasSlot, int32 ZOrder)
	{
		if (!CanvasSlot)
			return;
		CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		CanvasSlot->SetOffsets(FMargin(0.0f));
		CanvasSlot->SetZOrder(ZOrder);
	};
	FillCanvas(Root->AddChildToCanvas(SceneCanvas), -1000);
	if (OldRoot)
		FillCanvas(Root->AddChildToCanvas(OldRoot), 0);
	WidgetTree->RootWidget = Root;
	VNSCENE_LOG("created SceneCanvas behind %s.", *GetNameSafe(OldRoot));
}

void UKishiRpySceneManagerWidgetBase::SetSceneManagerState_Implementation(const FRpySceneManagerState &State)
{
	UKishiShowableLayer *SceneLayer = GetLayer();
	SceneLayer->Reset();
	BackgroundTag = NAME_None;
	TGuardValue<bool> Restoring(SceneLayer->bRestoringState, true);
	URpySceneManagerLibrary::SetSceneManagerState_Default(this, State);
}

ERpyInstructionResult UKishiRpySceneManagerWidgetBase::Scene_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	VNSCENE_LOG("scene tag='%s' asset=%s", *Image.tag.ToString(), *Image.asset.ToString());
	SceneState.rpyImage = Image;
	SceneState.options = Options;
	// Anchor and camera transition first: "scene _" with a transition still moves the camera.
	ApplySceneCameraTransition(Options);
	UKishiShowableLayer *SceneLayer = GetLayer();
	ERpyInstructionResult Result = ERpyInstructionResult::Continue;
	if (Image.asset.IsNull())
	{
		// "scene _" clears the background, with its transition.
		if (!BackgroundTag.IsNone())
			Result = SceneLayer->Hide(BackgroundTag, Options);
		BackgroundTag = NAME_None;
		OnSceneRequested(Image, Options);
		return Result;
	}
	UObject *Asset = Image.LoadAsset();
	if (ULevelSequence *Sequence = Cast<ULevelSequence>(Asset))
	{
		// A level sequence image replaces any transition sequence started above, and keeps the background.
		Result = PlaySceneSequence(Sequence, FindSceneCamera()) ? ERpyInstructionResult::Continue : ERpyInstructionResult::Failed;
		OnSceneRequested(Image, Options);
		return Result;
	}
	const TSubclassOf<UObject> ShowableClass = URpyShowManagerLibrary::ResolveShowableClass(Asset, EKishiShowableSpace::Widget, ShowableClassOverrides);
	// A new tag for every background, so the previous one stays under it while its transition plays.
	FRpyImage Background = Image;
	Background.tag = FName(TEXT("Background"), ++BackgroundCount);
	Result = SceneLayer->Show(Background, Options, ShowableClass);
	if (Result == ERpyInstructionResult::Failed)
	{
		// Reported as Failed; the session applies the Failed Instruction Policy, and the previous background stays.
		UE_LOG(LogTemp, Error, TEXT("%s: could not display scene image '%s' (%s)."), *GetName(), *Image.tag.ToString(), *Image.asset.ToString());
		SceneLayer->RemoveShowable(Background.tag);
	}
	else
	{
		if (!BackgroundTag.IsNone())
			SceneLayer->RetireShowable(BackgroundTag, URpyShowManagerLibrary::MakeShowableOptions(Options).Duration, Background.tag);
		BackgroundTag = Background.tag;
	}
	OnSceneRequested(Image, Options);
	return Result;
}

void UKishiRpySceneManagerWidgetBase::OnSkipRequested_Implementation()
{
	if (ShowableLayer)
		ShowableLayer->Skip();
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
	if (ShowableLayer)
		ShowableLayer->Reset();
	BackgroundTag = NAME_None;
	SceneState = FRpySceneManagerState();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}
