#include "Widgets/KishiRpyShowManagerWidgetBase.h"
#include "Interfaces/RpyScriptInterpreterBaseManager.h"
#include "Interfaces/RpyScriptInterpreter.h"
#include "Interfaces/LayeredSpriteHandler.h"
#include "Interfaces/Animatable.h"
#include "Assets/VideoMediaSprite.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "UObject/UnrealType.h"
#include "KishiRpyWidgetReflection.h"
#include "Animation/KishiAnimationComponent.h"

// Temporary show lifecycle tracing. Filter the Output Log on "[VNShow]".
#define VNSHOW_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: " Format), *GetName(), ##__VA_ARGS__)

namespace
{
	FString DescribeImage(const FRpyImage &Image)
	{
		return FString::Printf(TEXT("tag='%s' type=%s sprite=%s layered=%s video=%s"),
													 *Image.tag.ToString(), *UEnum::GetValueAsString(Image.type), *GetNameSafe(Image.sprite),
													 *GetNameSafe(Image.layeredSprite), *GetNameSafe(Image.videoMediaSprite));
	}

	FString DescribeOptions(const FRpySceneOptions &Options)
	{
		return FString::Printf(TEXT("attribute='%s' with=%s time=%.2f positionType=%s positionName='%s' position=%s zorder=%d"),
													 *Options.attribute, *UEnum::GetValueAsString(Options.with), Options.transitionTime,
													 *UEnum::GetValueAsString(Options.positionType), *Options.positionName.ToString(),
													 *Options.position.ToString(), Options.zorder);
	}

	FString DescribeWidget(const UObject *Object)
	{
		const UWidget *Widget = Cast<UWidget>(Object);
		if (!Widget)
			return FString::Printf(TEXT("%s (not a widget)"), *GetNameSafe(Object));
		FString Result = FString::Printf(TEXT("%s class=%s visibility=%s opacity=%.2f renderTranslation=%s parent=%s desiredSize=%s"),
																		 *Widget->GetName(), *Widget->GetClass()->GetName(), *UEnum::GetValueAsString(Widget->GetVisibility()),
																		 Widget->GetRenderOpacity(), *Widget->RenderTransform.Translation.ToString(),
																		 *GetNameSafe(Widget->GetParent()), *Widget->GetDesiredSize().ToString());
		if (const UCanvasPanelSlot *CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			const FAnchors Anchors = CanvasSlot->GetAnchors();
			const FMargin Offsets = CanvasSlot->GetOffsets();
			Result += FString::Printf(TEXT(" anchors=(%s - %s) offsets=(L%.0f T%.0f R%.0f B%.0f) zorder=%d"),
																*Anchors.Minimum.ToString(), *Anchors.Maximum.ToString(),
																Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom, CanvasSlot->GetZOrder());
		}
		return Result;
	}
}

void UKishiRpyShowManagerWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsDesignTime())
		return;
	VNSHOW_LOG("initialized. class=%s ShowCanvas=%s SpriteWidgetClass=%s LayeredSpriteWidgetClass=%s ShowTargetSize=%s",
						 *GetClass()->GetName(), *GetNameSafe(ShowCanvas), *GetNameSafe(SpriteWidgetClass.Get()),
						 *GetNameSafe(LayeredSpriteWidgetClass.Get()), *ShowTargetSize.ToString());
	if (ShowCanvas)
		VNSHOW_LOG("ShowCanvas %s", *DescribeWidget(ShowCanvas));
	for (const FName FunctionName : {FName(TEXT("Show")), FName(TEXT("Hide")), FName(TEXT("SetShowManagerState")), FName(TEXT("Reset"))})
	{
		const UFunction *Function = GetClass()->FindFunctionByName(FunctionName);
		if (Function && Function->GetOuterUClass() != UKishiRpyShowManagerWidgetBase::StaticClass() && !Function->GetOuterUClass()->HasAnyClassFlags(CLASS_Interface | CLASS_Native))
			UE_LOG(LogTemp, Warning, TEXT("[VNShow] %s: the Blueprint %s overrides %s; the C++ implementation will not run unless it calls the parent node."),
						 *GetName(), *Function->GetOuterUClass()->GetName(), *FunctionName.ToString());
	}
}

void UKishiRpyShowManagerWidgetBase::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// Refit when the canvas is resized, and once it has a size if images were shown before its first layout.
	const FVector2D CanvasSize = ShowCanvas ? ShowCanvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	if (!CanvasSize.Equals(FittedCanvasSize, 0.5f))
	{
		FittedCanvasSize = CanvasSize;
		VNSHOW_LOG("ShowCanvas size is now %s, refitting %d showables.", *CanvasSize.ToString(), TrackedShowables.Num());
		for (const TPair<FName, UObject *> &Pair : TrackedShowables)
			PendingFits.Add(Pair.Key);
	}
	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return;

	const TMap<FName, FRpySceneOptions> Transitions = MoveTemp(PendingShowTransitions);
	PendingShowTransitions.Reset();
	for (const TPair<FName, FRpySceneOptions> &Pair : Transitions)
	{
		UObject *Showable = FindShowable(Pair.Key);
		if (!Showable)
			continue;
		VNSHOW_LOG("ShowCanvas has a size, starting the waiting show transition of '%s'.", *Pair.Key.ToString());
		FitWidgetShowable(Showable);
		PendingFits.Remove(Pair.Key);
		PlayShowableTransition(Showable, Pair.Value, true, FVector2D::ZeroVector, true);
	}

	for (auto It = PendingFits.CreateIterator(); It; ++It)
	{
		UObject *Showable = FindShowable(*It);
		if (Showable && Showable->GetClass()->ImplementsInterface(UAnimatable::StaticClass()) && IAnimatable::Execute_IsAnimating(Showable))
			continue;
		FitWidgetShowable(Showable);
		It.RemoveCurrent();
	}
}

void UKishiRpyShowManagerWidgetBase::SetShowManagerState_Implementation(const FRpyShowManagerState &State)
{
	VNSHOW_LOG("SetShowManagerState with %d images.", State.imageStates.Num());
	const FRpyShowManagerState Saved = State;
	RemoveAllShowables();
	ShowState.imageStates.Reset();
	for (const FRpyShowImageState &ImageState : Saved.imageStates)
	{
		IRpyShowManager::Execute_Show(this, ImageState.rpyImage, ImageState.options);
	}
}

bool UKishiRpyShowManagerWidgetBase::Show_Implementation(FRpyImage Image, FRpySceneOptions Options)
{
	VNSHOW_LOG("Show begin. %s | %s", *DescribeImage(Image), *DescribeOptions(Options));
	FRpyShowImageState *Existing = ShowState.imageStates.FindByPredicate([&Image](const FRpyShowImageState &Item)
																																			 { return Item.name == Image.tag; });
	if (!Existing)
	{
		Existing = &ShowState.imageStates.AddDefaulted_GetRef();
	}
	Existing->name = Image.tag;
	Existing->rpyImage = Image;
	Existing->options = Options;
	ClearHideTimer(Image.tag);

	// Errors below are logged and the script continues, so a broken image never stalls it.
	const TSubclassOf<UObject> ShowableClass = ResolveShowableClass(Image);
	UObject *Showable = FindShowable(Image.tag);
	FVector2D MoveFrom = FVector2D::ZeroVector;
	bool bEnters = true;
	VNSHOW_LOG("Show resolved class=%s existing=%s", *GetNameSafe(ShowableClass.Get()), *GetNameSafe(Showable));
	if (CanReuseShowable(Showable, ShowableClass, Image))
	{
		VNSHOW_LOG("Show reusing %s", *GetNameSafe(Showable));
		bEnters = !IShowable::Execute_IsShown(Showable);
		// Same layered sprite: switch attributes and position in place instead of recreating it.
		if (Options.positionType != EPositionType::NONE)
		{
			const FKishiShowableOptions ShowableOptions = MakeShowableOptions(Options);
			// Fades, zoom, wipe and blur reveal in place instead of travelling.
			const bool bRevealsInPlace = ShowableOptions.bAnimateOpacity || ShowableOptions.Type == EKishiAnimationType::Zoom ||
																	 ShowableOptions.Type == EKishiAnimationType::Wipe || ShowableOptions.Type == EKishiAnimationType::Blur;
			if (!bEnters && bRevealsInPlace && ShowableOptions.Duration > 0.0f && !IsWidgetShowableAt(Showable, FVector2D(Options.position.X, Options.position.Y)) && GetWorld())
			{
				// Play the transition out where it is, then move and play it in with the new attributes.
				FRpySceneOptions HalfOptions = Options;
				HalfOptions.transitionTime = ShowableOptions.Duration * 0.5f;
				FKishiShowableOptions HideOptions = MakeShowableOptions(HalfOptions);
				HideOptions.Attributes = FString();
				VNSHOW_LOG("Show '%s' transitions out before moving, then in at the new position.", *Image.tag.ToString());
				IShowable::Execute_Hide(Showable, HideOptions);
				const FName Tag = Image.tag;
				FTimerHandle &Handle = HideTimers.Add(Tag);
				GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, Tag, HalfOptions]()
																																								{
					HideTimers.Remove(Tag);
					UObject *Moving = FindShowable(Tag);
					if (!Moving)
						return;
					// Finish the transition out first, so the layered sprite takes the new attributes while hidden.
					if (IAnimatable::Execute_IsAnimating(Moving))
						IAnimatable::Execute_Skip(Moving);
					PlaceWidgetShowable(Moving, FVector2D(HalfOptions.position.X, HalfOptions.position.Y), HalfOptions.zorder);
					FitWidgetShowable(Moving);
					PendingFits.Remove(Tag);
					PlayShowableTransition(Moving, HalfOptions, true, FVector2D::ZeroVector, true); }),
																							 HalfOptions.transitionTime, false);
				OnShowRequested(Image, Options);
				return true;
			}
			MoveFrom = PlaceWidgetShowable(Showable, FVector2D(Options.position.X, Options.position.Y), Options.zorder);
		}
	}
	else
	{
		if (!ShowableClass)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: no showable class for image '%s' of type %s. Set SpriteWidgetClass and LayeredSpriteWidgetClass, or override ResolveShowableClass."),
						 *GetName(), *Image.tag.ToString(), *UEnum::GetValueAsString(Image.type));
			OnShowRequested(Image, Options);
			return true;
		}
		Showable = CreateShowableFromClass(ShowableClass, Image.tag, Options.position, FRotator::ZeroRotator, FVector2D(Options.position.X, Options.position.Y), Options.zorder);
		if (!Showable)
		{
			OnShowRequested(Image, Options);
			return true;
		}
		VNSHOW_LOG("Show created %s", *DescribeWidget(Showable));
		if (!ApplyImageToShowable(Showable, Image))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: could not give image '%s' of type %s to showable %s."),
						 *GetName(), *Image.tag.ToString(), *UEnum::GetValueAsString(Image.type), *GetNameSafe(Showable));
		}
		else
		{
			VNSHOW_LOG("Show applied image to %s", *GetNameSafe(Showable));
		}
	}
	UWidget *ShowableWidget = Cast<UWidget>(Showable);
	const FVector2D CanvasSize = ShowCanvas ? ShowCanvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	if (ShowableWidget && ShowCanvas && (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f))
	{
		// Shown before ShowCanvas was laid out: keep it hidden but measured, and start the transition once it can be fitted.
		ShowableWidget->SetVisibility(ESlateVisibility::Hidden);
		PendingShowTransitions.Add(Image.tag, Options);
		VNSHOW_LOG("Show '%s' waits for ShowCanvas to get a size before its transition.", *Image.tag.ToString());
	}
	else
	{
		// Fit before the transition, so the animation starts from and returns to the fitted scale.
		FitWidgetShowable(Showable);
		PendingFits.Remove(Image.tag);
		PlayShowableTransition(Showable, Options, true, MoveFrom, bEnters);
	}
	OnShowRequested(Image, Options);
	VNSHOW_LOG("Show end. %s | canvas children=%d tracked=%d", *DescribeWidget(Showable), ShowCanvas ? ShowCanvas->GetChildrenCount() : -1, TrackedShowables.Num());
	if (UWidget *Widget = Cast<UWidget>(Showable))
	{
		// Desired size and geometry are only known after the next layout pass.
		if (UWorld *World = GetWorld())
		{
			TWeakObjectPtr<UWidget> WeakWidget = Widget;
			const FName Tag = Image.tag;
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, WeakWidget, Tag]()
																																										{
				if (!WeakWidget.IsValid())
				{
					VNSHOW_LOG("Show next tick: showable for '%s' is gone.", *Tag.ToString());
					return;
				}
				const FGeometry &Geometry = WeakWidget->GetCachedGeometry();
				VNSHOW_LOG("Show next tick '%s': %s | localSize=%s absolutePos=%s inViewport=%d managerVisibility=%s",
									 *Tag.ToString(), *DescribeWidget(WeakWidget.Get()), *Geometry.GetLocalSize().ToString(),
									 *Geometry.GetAbsolutePosition().ToString(), IsInViewport() ? 1 : 0, *UEnum::GetValueAsString(GetVisibility())); }));
		}
	}
	return true;
}

bool UKishiRpyShowManagerWidgetBase::Hide_Implementation(FName Tag, FRpySceneOptions Options)
{
	VNSHOW_LOG("Hide '%s' | %s", *Tag.ToString(), *DescribeOptions(Options));
	const int32 Index = ShowState.imageStates.IndexOfByPredicate([Tag](const FRpyShowImageState &Item)
																															 { return Item.name == Tag || Item.rpyImage.tag == Tag; });
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: hide '%s' ignored, no image with that tag is shown."), *GetName(), *Tag.ToString());
		return true;
	}
	OnHideRequested(Tag, Options);
	const FName StateTag = ShowState.imageStates[Index].name;
	ShowState.imageStates.RemoveAt(Index);

	UObject *Showable = FindShowable(StateTag);
	const float Duration = MakeShowableOptions(Options).Duration;
	if (!IsValid(Showable) || Duration <= 0.0f || !GetWorld())
	{
		RemoveShowable(StateTag);
		return true;
	}
	PlayShowableTransition(Showable, Options, false);
	ClearHideTimer(StateTag);
	FTimerHandle &Handle = HideTimers.Add(StateTag);
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, StateTag]()
																																					{ HideTimers.Remove(StateTag); RemoveShowable(StateTag); }),
																				 Duration, false);
	return true;
}

void UKishiRpyShowManagerWidgetBase::Reset_Implementation()
{
	VNSHOW_LOG("Reset, removing %d showables.", TrackedShowables.Num());
	RemoveAllShowables();
	ShowState.imageStates.Reset();
	UKishiRpyManagerWidgetBase::Reset_Implementation();
}

void UKishiRpyShowManagerWidgetBase::OnDialogueViseme_Implementation(FName Name, EMouthViseme Viseme, float TimeOffset)
{
	UKishiRpyManagerWidgetBase::OnDialogueViseme_Implementation(Name, Viseme, TimeOffset);
	UObject *Speaker = FindShowable(Name);
	if (IsValid(Speaker) && Speaker->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
		ILayeredSpriteHandler::Execute_OnSpeakerViseme(Speaker, Viseme, TimeOffset);
}

TArray<FRpyShowManagerState> UKishiRpyShowManagerWidgetBase::GetCurrentShowStates_Implementation() const
{
	return {ShowState};
}

UObject *UKishiRpyShowManagerWidgetBase::CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder)
{
	if (!ShowableClass || !ShowableClass->ImplementsInterface(UShowable::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot create showable '%s', class %s does not implement IShowable."), *GetName(), *Tag.ToString(), *GetNameSafe(ShowableClass.Get()));
		return nullptr;
	}
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot create showable '%s' without a world."), *GetName(), *Tag.ToString());
		return nullptr;
	}
	RemoveShowable(Tag);
	VNSHOW_LOG("CreateShowableFromClass '%s' class=%s widgetPosition=%s zorder=%d", *Tag.ToString(), *ShowableClass->GetName(), *WidgetPosition.ToString(), ZOrder);

	UObject *Showable = nullptr;
	if (ShowableClass->IsChildOf(UUserWidget::StaticClass()))
	{
		if (!ShowCanvas)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: cannot show widget '%s', add a Canvas Panel named ShowCanvas to this widget."), *GetName(), *Tag.ToString());
			return nullptr;
		}
		TSubclassOf<UUserWidget> WidgetClass = ShowableClass.Get();
		UUserWidget *Widget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (!Widget || !ShowCanvas->AddChildToCanvas(Widget))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: could not create widget %s for '%s'."), *GetName(), *GetNameSafe(ShowableClass.Get()), *Tag.ToString());
			return nullptr;
		}
		PlaceWidgetShowable(Widget, WidgetPosition, ZOrder);
		Showable = Widget;
	}
	else if (ShowableClass->IsChildOf(AActor::StaticClass()))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Showable = GetWorld()->SpawnActor<AActor>(ShowableClass.Get(), WorldLocation, WorldRotation, SpawnParams);
	}
	else if (ShowableClass->IsChildOf(UActorComponent::StaticClass()))
	{
		AActor *Owner = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), WorldLocation, WorldRotation);
		if (!Owner)
			return nullptr;
		USceneComponent *Root = NewObject<USceneComponent>(Owner, TEXT("ShowableRoot"));
		Owner->SetRootComponent(Root);
		Owner->AddInstanceComponent(Root);
		Root->RegisterComponent();
		UActorComponent *Component = NewObject<UActorComponent>(Owner, ShowableClass.Get());
		Owner->AddInstanceComponent(Component);
		Component->RegisterComponent();
		if (UKishiAnimationComponent *AnimationComponent = Cast<UKishiAnimationComponent>(Component))
		{
			AnimationComponent->WorldTarget = Root;
		}
		Showable = Component;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: showable class %s must derive from UUserWidget, AActor or UActorComponent."), *GetName(), *GetNameSafe(ShowableClass.Get()));
		return nullptr;
	}

	if (!IsValid(Showable))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create showable %s for '%s'."), *GetName(), *GetNameSafe(ShowableClass.Get()), *Tag.ToString());
		return nullptr;
	}
	TrackedShowables.Add(Tag, Showable);
	if (FNameProperty *TagProperty = FindFProperty<FNameProperty>(Showable->GetClass(), TEXT("Tag")))
	{
		TagProperty->SetPropertyValue_InContainer(Showable, Tag);
	}
	return Showable;
}

UObject *UKishiRpyShowManagerWidgetBase::FindShowable(FName Tag) const
{
	UObject *const *Found = TrackedShowables.Find(Tag);
	return Found && IsValid(*Found) ? *Found : nullptr;
}

bool UKishiRpyShowManagerWidgetBase::ShowTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	UObject *Showable = FindShowable(Tag);
	if (!Showable)
		return false;
	IShowable::Execute_Show(Showable, Options);
	return true;
}

bool UKishiRpyShowManagerWidgetBase::HideTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	UObject *Showable = FindShowable(Tag);
	if (!Showable)
		return false;
	IShowable::Execute_Hide(Showable, Options);
	return true;
}

bool UKishiRpyShowManagerWidgetBase::RemoveShowable(FName Tag)
{
	UObject **Found = TrackedShowables.Find(Tag);
	if (!Found)
		return false;
	UObject *Object = *Found;
	VNSHOW_LOG("RemoveShowable '%s' (%s)", *Tag.ToString(), *GetNameSafe(Object));
	TrackedShowables.Remove(Tag);
	PendingFits.Remove(Tag);
	PendingShowTransitions.Remove(Tag);
	if (UUserWidget *Widget = Cast<UUserWidget>(Object))
	{
		Widget->RemoveFromParent();
	}
	else if (AActor *Actor = Cast<AActor>(Object))
	{
		Actor->Destroy();
	}
	else if (UActorComponent *Component = Cast<UActorComponent>(Object))
	{
		if (AActor *Owner = Component->GetOwner())
			Owner->Destroy();
		else
			Component->DestroyComponent();
	}
	return true;
}

FKishiShowableOptions UKishiRpyShowManagerWidgetBase::MakeShowableOptions(const FRpySceneOptions &Options)
{
	FKishiShowableOptions Result;
	Result.Attributes = Options.attribute;
	Result.Space = EKishiAnimationSpace::ParentWidget;
	Result.Duration = Options.transitionTime;
	Result.bAnimateOpacity = false;
	switch (Options.with)
	{
	case ERPYTransitionType::FADE:
		Result.Type = EKishiAnimationType::Fade;
		Result.bAnimateOpacity = true;
		break;
	case ERPYTransitionType::DISSOLVE:
		Result.Type = EKishiAnimationType::Dissolve;
		Result.bAnimateOpacity = true;
		break;
	case ERPYTransitionType::BLUR:
		Result.Type = EKishiAnimationType::Blur;
		break;
	case ERPYTransitionType::EASE:
		Result.Type = EKishiAnimationType::Ease;
		break;
	case ERPYTransitionType::MOVE:
		Result.Type = EKishiAnimationType::Move;
		break;
	case ERPYTransitionType::SHAKE:
		Result.Type = EKishiAnimationType::Shake;
		break;
	case ERPYTransitionType::WIPE:
		Result.Type = EKishiAnimationType::Wipe;
		break;
	case ERPYTransitionType::ZOOM:
		Result.Type = EKishiAnimationType::Zoom;
		break;
	default:
		// NONE and LEVEL_SEQUENCE have no showable animation.
		Result.Type = EKishiAnimationType::Linear;
		Result.Duration = 0.0f;
		break;
	}
	return Result;
}

TSubclassOf<UObject> UKishiRpyShowManagerWidgetBase::ResolveShowableClass_Implementation(const FRpyImage &Image) const
{
	switch (Image.type)
	{
	case ERpyImageType::ELAYERED_SPRITE:
		return LayeredSpriteWidgetClass;
	case ERpyImageType::ESPRITE:
	case ERpyImageType::EVIDEO_MEDIA_SPRITE:
		return SpriteWidgetClass;
	default:
		return nullptr;
	}
}

bool UKishiRpyShowManagerWidgetBase::ApplyImageToShowable_Implementation(UObject *Showable, const FRpyImage &Image)
{
	if (!IsValid(Showable))
		return false;
	VNSHOW_LOG("ApplyImageToShowable %s: hasSetSprite=%d implementsLayeredSpriteHandler=%d", *Showable->GetName(),
						 Showable->FindFunction(FName(TEXT("SetSprite"))) ? 1 : 0, Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()) ? 1 : 0);
	switch (Image.type)
	{
	case ERpyImageType::ELAYERED_SPRITE:
		if (!IsValid(Image.layeredSprite) || !Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()))
			return false;
		ILayeredSpriteHandler::Execute_SetSprite(Showable, Image.layeredSprite);
		return true;
	case ERpyImageType::ESPRITE:
		return IsValid(Image.sprite) && SetImageOnWidget(Cast<UUserWidget>(Showable), Image.sprite, ShowTargetSize);
	case ERpyImageType::EVIDEO_MEDIA_SPRITE:
		return IsValid(Image.videoMediaSprite) && IsValid(Image.videoMediaSprite->fileMediaSource) &&
					 SetMediaOnWidget(Cast<UUserWidget>(Showable), Image.videoMediaSprite->fileMediaSource, ShowTargetSize);
	default:
		return false;
	}
}

bool UKishiRpyShowManagerWidgetBase::CanReuseShowable(UObject *Showable, TSubclassOf<UObject> ShowableClass, const FRpyImage &Image) const
{
	// Sprites and videos are recreated so the new image runs its own show transition.
	if (!IsValid(Showable) || !ShowableClass || Showable->GetClass() != ShowableClass.Get() || Image.type != ERpyImageType::ELAYERED_SPRITE)
		return false;
	return Showable->GetClass()->ImplementsInterface(ULayeredSpriteHandler::StaticClass()) &&
				 ILayeredSpriteHandler::Execute_GetSprite(Showable) == Image.layeredSprite;
}

FVector2D UKishiRpyShowManagerWidgetBase::PlaceWidgetShowable(UObject *Showable, FVector2D Position, int32 ZOrder) const
{
	UWidget *Widget = Cast<UWidget>(Showable);
	UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (!CanvasSlot)
		return FVector2D::ZeroVector;
	const FVector2D OldPosition = CanvasSlot->GetAnchors().Minimum - FVector2D(0.5f, 1.0f);
	// Script positions are fractions of the screen from the bottom center. Size the widget to its
	// content and pin its bottom center there, so it follows the canvas at any resolution.
	const FVector2D Anchor = FVector2D(0.5f, 1.0f) + Position;
	CanvasSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
	CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetOffsets(FMargin(0.0f));
	CanvasSlot->SetZOrder(ZOrder);
	const FVector2D CanvasSize = ShowCanvas ? ShowCanvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	return (OldPosition - Position) * CanvasSize;
}

bool UKishiRpyShowManagerWidgetBase::IsWidgetShowableAt(UObject *Showable, FVector2D Position) const
{
	const UWidget *Widget = Cast<UWidget>(Showable);
	const UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	return !CanvasSlot || CanvasSlot->GetAnchors().Minimum.Equals(FVector2D(0.5f, 1.0f) + Position, KINDA_SMALL_NUMBER);
}

FVector2D UKishiRpyShowManagerWidgetBase::GetOffscreenOffset(UObject *Showable, ERPYTransitionDirection Direction) const
{
	const UWidget *Widget = Cast<UWidget>(Showable);
	const UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (!CanvasSlot || !ShowCanvas || Direction == ERPYTransitionDirection::NONE)
		return FVector2D::ZeroVector;
	const FVector2D CanvasSize = ShowCanvas->GetCachedGeometry().GetLocalSize();
	// The slot pins the widget's bottom center to its anchor; the render scale shrinks it around that point.
	const FVector2D Pin = CanvasSlot->GetAnchors().Minimum * CanvasSize;
	const FVector2D Size = Widget->GetDesiredSize() * Widget->RenderTransform.Scale;
	switch (Direction)
	{
	case ERPYTransitionDirection::LEFT:
		return FVector2D(-(Pin.X + Size.X * 0.5f), 0.0f);
	case ERPYTransitionDirection::RIGHT:
		return FVector2D(CanvasSize.X - Pin.X + Size.X * 0.5f, 0.0f);
	case ERPYTransitionDirection::UP:
		return FVector2D(0.0f, -Pin.Y);
	case ERPYTransitionDirection::DOWN:
		return FVector2D(0.0f, CanvasSize.Y - Pin.Y + Size.Y);
	default:
		return FVector2D::ZeroVector;
	}
}

void UKishiRpyShowManagerWidgetBase::FitWidgetShowable(UObject *Showable) const
{
	UWidget *Widget = Cast<UWidget>(Showable);
	if (!IsValid(Widget) || !ShowCanvas || !Cast<UCanvasPanelSlot>(Widget->Slot))
		return;
	const FVector2D CanvasSize = ShowCanvas->GetCachedGeometry().GetLocalSize();
	// The content was just set, so measure it now instead of waiting for the next layout pass.
	Widget->ForceLayoutPrepass();
	const FVector2D ContentSize = Widget->GetDesiredSize();
	float Scale = 1.0f;
	if (bFitShowablesToCanvas && CanvasSize.X > 0.0f && CanvasSize.Y > 0.0f && ContentSize.X > 0.0f && ContentSize.Y > 0.0f)
		Scale = FMath::Min3(1.0f, CanvasSize.X / ContentSize.X, CanvasSize.Y / ContentSize.Y);
	// Scale around the bottom center, the point the slot pins to the script position.
	Widget->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
	Widget->SetRenderScale(FVector2D(Scale));
	VNSHOW_LOG("fit %s: content=%s canvas=%s scale=%.3f", *Widget->GetName(), *ContentSize.ToString(), *CanvasSize.ToString(), Scale);
}

void UKishiRpyShowManagerWidgetBase::PlayShowableTransition(UObject *Showable, const FRpySceneOptions &Options, bool bShow, FVector2D MoveFrom, bool bEnters)
{
	if (!IsValid(Showable) || !Showable->GetClass()->ImplementsInterface(UShowable::StaticClass()))
		return;
	FKishiShowableOptions ShowableOptions = MakeShowableOptions(Options);
	const bool bTravels = ShowableOptions.Type == EKishiAnimationType::Move || ShowableOptions.Type == EKishiAnimationType::Ease;
	if (bShow && bTravels && bEnters && Options.direction != ERPYTransitionDirection::NONE)
	{
		// An entering image slides in from past the canvas edge named by the transition, such as easeinright.
		ShowableOptions.From = FVector(GetOffscreenOffset(Showable, Options.direction), 0.0f);
		ShowableOptions.bUseFrom = true;
	}
	else if (bShow && bTravels && !MoveFrom.IsNearlyZero())
	{
		// The slot already sits at the new position; animate the render offset from the old one back to zero.
		ShowableOptions.From = FVector(MoveFrom, 0.0f);
		ShowableOptions.bUseFrom = true;
	}
	else if (!bShow && bTravels)
	{
		// A hidden image slides out past the canvas edge named by the transition.
		ShowableOptions.To = FVector(GetOffscreenOffset(Showable, Options.direction), 0.0f);
	}
	VNSHOW_LOG("%s transition on %s: type=%s duration=%.2f animateOpacity=%d useFrom=%d from=%s", bShow ? TEXT("show") : TEXT("hide"),
						 *Showable->GetName(), *UEnum::GetValueAsString(ShowableOptions.Type), ShowableOptions.Duration,
						 ShowableOptions.bAnimateOpacity ? 1 : 0, ShowableOptions.bUseFrom ? 1 : 0, *ShowableOptions.From.ToString());
	if (bShow)
		IShowable::Execute_Show(Showable, ShowableOptions);
	else
		IShowable::Execute_Hide(Showable, ShowableOptions);
}

void UKishiRpyShowManagerWidgetBase::ClearHideTimer(FName Tag)
{
	if (FTimerHandle *HideTimer = HideTimers.Find(Tag))
	{
		if (GetWorld())
			GetWorld()->GetTimerManager().ClearTimer(*HideTimer);
		HideTimers.Remove(Tag);
	}
}

void UKishiRpyShowManagerWidgetBase::RemoveAllShowables()
{
	if (GetWorld())
	{
		for (TPair<FName, FTimerHandle> &Pair : HideTimers)
			GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
	}
	HideTimers.Reset();
	TArray<FName> Tags;
	TrackedShowables.GetKeys(Tags);
	for (FName Tag : Tags)
		RemoveShowable(Tag);
}
