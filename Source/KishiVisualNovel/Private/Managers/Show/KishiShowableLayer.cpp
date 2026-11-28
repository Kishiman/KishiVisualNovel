#include "Managers/Show/KishiShowableLayer.h"

#include "Animation/KishiAnimationComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Managers/Show/RpyShowManager.h"
#include "Player/RpyScriptInterpreterBaseManager.h"
#include "Script/RpyInstruction.h"
#include "Showables/Animatable.h"
#include "TimerManager.h"
#include "UObject/UnrealType.h"

// Showable lifecycle tracing. Filter the Output Log on "[VNShow]".
#define VNSHOW_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: " Format), *GetOwnerName(), ##__VA_ARGS__)

namespace
{
	bool IsShowableLoading(UObject *Object)
	{
		return IsValid(Object) && Object->GetClass()->ImplementsInterface(UShowable::StaticClass()) && IShowable::Execute_IsLoading(Object);
	}

	bool IsAnimating(UObject *Object)
	{
		return IsValid(Object) && Object->GetClass()->ImplementsInterface(UAnimatable::StaticClass()) && IAnimatable::Execute_IsAnimating(Object);
	}

	FString DescribeWidget(const UObject *Object)
	{
		const UWidget *Widget = Cast<UWidget>(Object);
		if (!Widget)
			return FString::Printf(TEXT("%s (not a widget)"), *GetNameSafe(Object));
		return FString::Printf(TEXT("%s class=%s visibility=%s opacity=%.2f renderTranslation=%s parent=%s desiredSize=%s"), *Widget->GetName(),
							   *Widget->GetClass()->GetName(), *UEnum::GetValueAsString(Widget->GetVisibility()), Widget->GetRenderOpacity(),
							   *Widget->RenderTransform.Translation.ToString(), *GetNameSafe(Widget->GetParent()), *Widget->GetDesiredSize().ToString());
	}
}

void UKishiShowableLayer::Initialize(UUserWidget *InOwner, UCanvasPanel *InCanvas)
{
	Owner = InOwner;
	Canvas = InCanvas;
}

UWorld *UKishiShowableLayer::GetWorld() const
{
	return Owner ? Owner->GetWorld() : nullptr;
}

FString UKishiShowableLayer::GetOwnerName() const
{
	return GetNameSafe(Owner);
}

ERpyInstructionResult UKishiShowableLayer::Show(const FRpyImage &Image, const FRpySceneOptions &Options, TSubclassOf<UObject> ShowableClass)
{
	ERpyInstructionResult Result = ERpyInstructionResult::Continue;
	if (HidingTags.Contains(Image.tag))
	{
		// Its hide is still playing: end it now, so this show enters instead of finding the image on screen, and report the hide as finished.
		VNSHOW_LOG("Show '%s' ends the hide that is still playing.", *Image.tag.ToString());
		UObject *Hiding = FindShowable(Image.tag);
		if (IsAnimating(Hiding))
			IAnimatable::Execute_Skip(Hiding);
		HidingTags.Remove(Image.tag);
		ClearHideTimer(Image.tag);
		FinishWaitsForTag(Image.tag);
	}
	ClearHideTimer(Image.tag);

	// Errors below are logged and the script continues, so a broken image never stalls it.
	UObject *Showable = FindShowable(Image.tag);
	FVector2D MoveFrom = FVector2D::ZeroVector;
	bool bEnters = true;
	VNSHOW_LOG("Show '%s' class=%s existing=%s", *Image.tag.ToString(), *GetNameSafe(ShowableClass.Get()), *GetNameSafe(Showable));
	if (URpyShowManagerLibrary::CanReuseShowable(Showable, ShowableClass, Image))
	{
		bEnters = !IShowable::Execute_IsShown(Showable);
		// Same layered sprite: switch attributes and position in place instead of recreating it.
		if (Options.positionType != EPositionType::NONE && Placement == EKishiShowablePlacement::BottomCenter)
		{
			const FKishiShowableOptions ShowableOptions = URpyShowManagerLibrary::MakeShowableOptions(Options);
			// Fades, zoom, wipe and blur reveal in place instead of travelling.
			const bool bRevealsInPlace = ShowableOptions.bAnimateOpacity || ShowableOptions.Type == EKishiAnimationType::Zoom ||
										 ShowableOptions.Type == EKishiAnimationType::Wipe || ShowableOptions.Type == EKishiAnimationType::Blur;
			if (!bEnters && bRevealsInPlace && ShowableOptions.Duration > 0.0f && !IsWidgetShowableAt(Showable, FVector2D(Options.position.X, Options.position.Y)) && GetWorld())
			{
				// Play the transition out where it is, then move and play it in with the new attributes.
				FRpySceneOptions HalfOptions = Options;
				HalfOptions.transitionTime = ShowableOptions.Duration * 0.5f;
				FKishiShowableOptions HideOptions = URpyShowManagerLibrary::MakeShowableOptions(HalfOptions);
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
					if (IsAnimating(Moving))
						IAnimatable::Execute_Skip(Moving);
					PlaceWidgetShowable(Moving, FVector2D(HalfOptions.position.X, HalfOptions.position.Y), HalfOptions.zorder);
					FitWidgetShowable(Moving);
					PendingFits.Remove(Tag);
					PlayShowableTransition(Moving, HalfOptions, true, FVector2D::ZeroVector, true); }),
													   HalfOptions.transitionTime, false);
				return BeginWait(Image.tag, ShowableOptions.Duration);
			}
			MoveFrom = PlaceWidgetShowable(Showable, FVector2D(Options.position.X, Options.position.Y), Options.zorder);
		}
	}
	else
	{
		if (!ShowableClass)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: no widget showable for image '%s' (%s). Add a row for its asset class to Showables in Project Settings > Plugins > Kishi Visual Novel, or to the manager's ShowableClassOverrides."),
				   *GetOwnerName(), *Image.tag.ToString(), *Image.asset.ToString());
			return ERpyInstructionResult::Failed;
		}
		Showable = CreateShowableFromClass(ShowableClass, Image.tag, Options.position, FRotator::ZeroRotator, FVector2D(Options.position.X, Options.position.Y), Options.zorder);
		if (!Showable)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: could not create showable %s for image '%s'."), *GetOwnerName(), *GetNameSafe(ShowableClass), *Image.tag.ToString());
			return ERpyInstructionResult::Failed;
		}
		if (!IShowable::Execute_SetShowableAsset(Showable, Image.LoadAsset(), TargetSize))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: could not give image '%s' (%s) to showable %s."), *GetOwnerName(), *Image.tag.ToString(), *Image.asset.ToString(), *GetNameSafe(Showable));
			Result = ERpyInstructionResult::Failed;
		}
	}
	UWidget *ShowableWidget = Cast<UWidget>(Showable);
	const FVector2D CanvasSize = Canvas ? Canvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	const bool bCanvasUnsized = ShowableWidget && Canvas && (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f);
	const bool bLoading = IsShowableLoading(Showable);
	if (bCanvasUnsized || bLoading)
	{
		// Shown before the canvas was laid out, or before its content can be drawn (a video before its first frame):
		// keep it hidden but measured, and start the transition in Tick once it can be fitted and drawn.
		if (ShowableWidget)
			ShowableWidget->SetVisibility(ESlateVisibility::Hidden);
		PendingShowTransitions.Add(Image.tag, Options);
		PendingShowStartTimes.Add(Image.tag, FPlatformTime::Seconds());
		VNSHOW_LOG("Show '%s' waits for %s before its transition.", *Image.tag.ToString(), bLoading ? TEXT("its content to load") : TEXT("the canvas to get a size"));
	}
	else
	{
		// Fit before the transition, so the animation starts from and returns to the fitted scale.
		FitWidgetShowable(Showable);
		PendingFits.Remove(Image.tag);
		PlayShowableTransition(Showable, Options, true, MoveFrom, bEnters);
	}
	VNSHOW_LOG("Show end. %s | tracked=%d", *DescribeWidget(Showable), TrackedShowables.Num());
	if (Result == ERpyInstructionResult::Failed)
		return Result;
	// The script waits for the transition; a click skips it (Skip).
	return BeginWait(Image.tag, URpyShowManagerLibrary::MakeShowableOptions(Options).Duration);
}

ERpyInstructionResult UKishiShowableLayer::Hide(FName Tag, const FRpySceneOptions &Options)
{
	UObject *Showable = FindShowable(Tag);
	const float Duration = URpyShowManagerLibrary::MakeShowableOptions(Options).Duration;
	if (!IsValid(Showable) || Duration <= 0.0f || !GetWorld())
	{
		RemoveShowable(Tag);
		return ERpyInstructionResult::Continue;
	}
	PlayShowableTransition(Showable, Options, false);
	ClearHideTimer(Tag);
	HidingTags.Add(Tag);
	FTimerHandle &Handle = HideTimers.Add(Tag);
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, Tag]()
																					{ FinishHide(Tag); }),
										   Duration, false);
	// The script waits for the hide; FinishHide reports it once the showable is removed.
	return BeginWait(Tag, Duration, false);
}

void UKishiShowableLayer::RetireShowable(FName Tag, float Delay, FName ReplacementTag)
{
	if (!FindShowable(Tag))
		return;
	if (!ReplacementTag.IsNone() && PendingShowTransitions.Contains(ReplacementTag))
	{
		// The replacement is not on screen yet; keep this one until its transition starts.
		RetiresAfterShow.FindOrAdd(ReplacementTag).Emplace(Tag, Delay);
		return;
	}
	if (Delay <= 0.0f || !GetWorld())
	{
		RemoveShowable(Tag);
		return;
	}
	ClearHideTimer(Tag);
	HidingTags.Add(Tag);
	FTimerHandle &Handle = HideTimers.Add(Tag);
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, Tag]()
																					{ FinishHide(Tag); }),
										   Delay, false);
}

void UKishiShowableLayer::Skip()
{
	VNSHOW_LOG("Skip: %d waits, %d removals pending.", Waits.Num(), HidingTags.Num());
	for (const TPair<FName, UObject *> &Pair : TrackedShowables)
		if (IsAnimating(Pair.Value))
			IAnimatable::Execute_Skip(Pair.Value);
	for (const FName Tag : HidingTags.Array())
		FinishHide(Tag);
	// Reporting a completion can run the next instructions, which start new waits; finish only the ones that were running.
	TArray<int32> Ids;
	Waits.GetKeys(Ids);
	for (const int32 Id : Ids)
		FinishWait(Id);
}

void UKishiShowableLayer::Reset()
{
	if (UWorld *World = GetWorld())
		for (TPair<FName, FTimerHandle> &Pair : HideTimers)
			World->GetTimerManager().ClearTimer(Pair.Value);
	HideTimers.Reset();
	HidingTags.Reset();
	RetiresAfterShow.Reset();
	TArray<FName> Tags;
	TrackedShowables.GetKeys(Tags);
	for (FName Tag : Tags)
		RemoveShowable(Tag);
	ClearWaits();
}

void UKishiShowableLayer::Tick()
{
	// Refit when the canvas is resized, and once it has a size if images were shown before its first layout.
	const FVector2D CanvasSize = Canvas ? Canvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	if (!CanvasSize.Equals(FittedCanvasSize, 0.5f))
	{
		FittedCanvasSize = CanvasSize;
		for (const TPair<FName, UObject *> &Pair : TrackedShowables)
			PendingFits.Add(Pair.Key);
	}
	if (CanvasSize.X <= 0.0f || CanvasSize.Y <= 0.0f)
		return;
	for (const TPair<FName, UObject *> &Pair : TrackedShowables)
	{
		const UWidget *Widget = Cast<UWidget>(Pair.Value);
		const FVector2D *Fitted = Widget ? FittedContentSizes.Find(Pair.Value) : nullptr;
		if (Fitted && !Widget->GetDesiredSize().Equals(*Fitted, 0.5f))
			PendingFits.Add(Pair.Key);
	}

	TArray<FName> ReadyTags;
	for (const TPair<FName, FRpySceneOptions> &Pair : PendingShowTransitions)
	{
		UObject *Showable = FindShowable(Pair.Key);
		if (IsShowableLoading(Showable))
		{
			const double *Start = PendingShowStartTimes.Find(Pair.Key);
			if (Start && FPlatformTime::Seconds() - *Start < LoadTimeout)
				continue;
			UE_LOG(LogTemp, Warning, TEXT("%s: '%s' (%s) is still loading after %.0f seconds; showing it anyway."), *GetOwnerName(), *Pair.Key.ToString(), *GetNameSafe(Showable), LoadTimeout);
		}
		ReadyTags.Add(Pair.Key);
	}
	for (const FName Tag : ReadyTags)
		StartPendingShowTransition(Tag);

	for (auto It = PendingFits.CreateIterator(); It; ++It)
	{
		UObject *Showable = FindShowable(*It);
		if (IsAnimating(Showable))
			continue;
		FitWidgetShowable(Showable);
		It.RemoveCurrent();
	}
}

UObject *UKishiShowableLayer::CreateShowableFromClass(TSubclassOf<UObject> ShowableClass, FName Tag, FVector WorldLocation, FRotator WorldRotation, FVector2D WidgetPosition, int32 ZOrder)
{
	if (!ShowableClass || !ShowableClass->ImplementsInterface(UShowable::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot create showable '%s', class %s does not implement IShowable."), *GetOwnerName(), *Tag.ToString(), *GetNameSafe(ShowableClass.Get()));
		return nullptr;
	}
	UWorld *World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: cannot create showable '%s' without a world."), *GetOwnerName(), *Tag.ToString());
		return nullptr;
	}
	RemoveShowable(Tag);

	UObject *Showable = nullptr;
	if (ShowableClass->IsChildOf(UUserWidget::StaticClass()))
	{
		if (!Canvas || !Owner)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: cannot show widget '%s' without a canvas; see the manager's canvas setting."), *GetOwnerName(), *Tag.ToString());
			return nullptr;
		}
		TSubclassOf<UUserWidget> WidgetClass = ShowableClass.Get();
		UUserWidget *Widget = CreateWidget<UUserWidget>(Owner, WidgetClass);
		if (!Widget || !Canvas->AddChildToCanvas(Widget))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: could not create widget %s for '%s'."), *GetOwnerName(), *GetNameSafe(ShowableClass.Get()), *Tag.ToString());
			return nullptr;
		}
		PlaceWidgetShowable(Widget, WidgetPosition, ZOrder);
		Showable = Widget;
	}
	else if (ShowableClass->IsChildOf(AActor::StaticClass()))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Showable = World->SpawnActor<AActor>(ShowableClass.Get(), WorldLocation, WorldRotation, SpawnParams);
	}
	else if (ShowableClass->IsChildOf(UActorComponent::StaticClass()))
	{
		AActor *ComponentOwner = World->SpawnActor<AActor>(AActor::StaticClass(), WorldLocation, WorldRotation);
		if (!ComponentOwner)
			return nullptr;
		USceneComponent *Root = NewObject<USceneComponent>(ComponentOwner, TEXT("ShowableRoot"));
		ComponentOwner->SetRootComponent(Root);
		ComponentOwner->AddInstanceComponent(Root);
		Root->RegisterComponent();
		UActorComponent *Component = NewObject<UActorComponent>(ComponentOwner, ShowableClass.Get());
		ComponentOwner->AddInstanceComponent(Component);
		Component->RegisterComponent();
		if (UKishiAnimationComponent *AnimationComponent = Cast<UKishiAnimationComponent>(Component))
			AnimationComponent->WorldTarget = Root;
		Showable = Component;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: showable class %s must derive from UUserWidget, AActor or UActorComponent."), *GetOwnerName(), *GetNameSafe(ShowableClass.Get()));
		return nullptr;
	}

	if (!IsValid(Showable))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: could not create showable %s for '%s'."), *GetOwnerName(), *GetNameSafe(ShowableClass.Get()), *Tag.ToString());
		return nullptr;
	}
	TrackedShowables.Add(Tag, Showable);
	if (FNameProperty *TagProperty = FindFProperty<FNameProperty>(Showable->GetClass(), TEXT("Tag")))
		TagProperty->SetPropertyValue_InContainer(Showable, Tag);
	return Showable;
}

UObject *UKishiShowableLayer::FindShowable(FName Tag) const
{
	UObject *const *Found = TrackedShowables.Find(Tag);
	return Found && IsValid(*Found) ? *Found : nullptr;
}

bool UKishiShowableLayer::ShowTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	UObject *Showable = FindShowable(Tag);
	if (!Showable)
		return false;
	IShowable::Execute_Show(Showable, Options);
	return true;
}

bool UKishiShowableLayer::HideTrackedShowable(FName Tag, const FKishiShowableOptions &Options)
{
	UObject *Showable = FindShowable(Tag);
	if (!Showable)
		return false;
	IShowable::Execute_Hide(Showable, Options);
	return true;
}

bool UKishiShowableLayer::RemoveShowable(FName Tag)
{
	UObject **Found = TrackedShowables.Find(Tag);
	if (!Found)
		return false;
	UObject *Object = *Found;
	VNSHOW_LOG("RemoveShowable '%s' (%s)", *Tag.ToString(), *GetNameSafe(Object));
	TrackedShowables.Remove(Tag);
	PendingFits.Remove(Tag);
	FittedContentSizes.Remove(Object);
	PendingShowTransitions.Remove(Tag);
	PendingShowStartTimes.Remove(Tag);
	// A replacement removed before it was shown: retire what it would have covered now, so nothing is left behind.
	TArray<TPair<FName, float>> Retires;
	if (RetiresAfterShow.RemoveAndCopyValue(Tag, Retires))
		for (const TPair<FName, float> &Retire : Retires)
			RetireShowable(Retire.Key, 0.0f);
	if (UUserWidget *Widget = Cast<UUserWidget>(Object))
		Widget->RemoveFromParent();
	else if (AActor *Actor = Cast<AActor>(Object))
		Actor->Destroy();
	else if (UActorComponent *Component = Cast<UActorComponent>(Object))
	{
		if (AActor *ComponentOwner = Component->GetOwner())
			ComponentOwner->Destroy();
		else
			Component->DestroyComponent();
	}
	return true;
}

ERpyInstructionResult UKishiShowableLayer::BeginWait(FName Tag, float Duration, bool bUseTimer)
{
	UWorld *World = GetWorld();
	URpyInstruction *Instruction = Owner ? URpyScriptInterpreterBaseManagerLibrary::GetExecutingInstruction(TScriptInterface<IRpyScriptInterpreterBaseManager>(Owner)) : nullptr;
	if (bRestoringState || Duration <= 0.0f || !World || !Instruction)
		return ERpyInstructionResult::Continue;
	const int32 Id = NextWaitId++;
	FShowableWait &Wait = Waits.Add(Id);
	Wait.Tag = Tag;
	Wait.Instruction = Instruction;
	if (bUseTimer)
		World->GetTimerManager().SetTimer(Wait.Timer, FTimerDelegate::CreateWeakLambda(this, [this, Id]()
																						{ FinishWait(Id); }),
										  Duration, false);
	return ERpyInstructionResult::Wait;
}

void UKishiShowableLayer::FinishWait(int32 Id)
{
	FShowableWait Wait;
	if (!Waits.RemoveAndCopyValue(Id, Wait))
		return;
	if (UWorld *World = GetWorld())
		World->GetTimerManager().ClearTimer(Wait.Timer);
	// The session ignores the completion if the script has already moved past the instruction.
	if (URpyInstruction *Instruction = Wait.Instruction.Get())
		if (Owner)
			URpyScriptInterpreterBaseManagerLibrary::ReportInstructionComplete(TScriptInterface<IRpyScriptInterpreterBaseManager>(Owner), Instruction);
}

void UKishiShowableLayer::FinishWaitsForTag(FName Tag)
{
	TArray<int32> Ids;
	for (const TPair<int32, FShowableWait> &Pair : Waits)
		if (Pair.Value.Tag == Tag)
			Ids.Add(Pair.Key);
	for (const int32 Id : Ids)
		FinishWait(Id);
}

void UKishiShowableLayer::StartPendingShowTransition(FName Tag)
{
	FRpySceneOptions Options;
	if (!PendingShowTransitions.RemoveAndCopyValue(Tag, Options))
		return;
	PendingShowStartTimes.Remove(Tag);
	if (UObject *Showable = FindShowable(Tag))
	{
		VNSHOW_LOG("Starting the waiting show transition of '%s'.", *Tag.ToString());
		FitWidgetShowable(Showable);
		PendingFits.Remove(Tag);
		PlayShowableTransition(Showable, Options, true, FVector2D::ZeroVector, true);
	}
	// The replacement is on screen now: start the retirements of what it covers.
	TArray<TPair<FName, float>> Retires;
	if (RetiresAfterShow.RemoveAndCopyValue(Tag, Retires))
		for (const TPair<FName, float> &Retire : Retires)
			RetireShowable(Retire.Key, Retire.Value);
}

void UKishiShowableLayer::FinishHide(FName Tag)
{
	ClearHideTimer(Tag);
	HidingTags.Remove(Tag);
	RemoveShowable(Tag);
	FinishWaitsForTag(Tag);
}

void UKishiShowableLayer::ClearHideTimer(FName Tag)
{
	if (FTimerHandle *HideTimer = HideTimers.Find(Tag))
	{
		if (UWorld *World = GetWorld())
			World->GetTimerManager().ClearTimer(*HideTimer);
		HideTimers.Remove(Tag);
	}
}

void UKishiShowableLayer::ClearWaits()
{
	if (UWorld *World = GetWorld())
		for (TPair<int32, FShowableWait> &Pair : Waits)
			World->GetTimerManager().ClearTimer(Pair.Value.Timer);
	Waits.Reset();
}

FVector2D UKishiShowableLayer::PlaceWidgetShowable(UObject *Showable, FVector2D Position, int32 ZOrder) const
{
	UWidget *Widget = Cast<UWidget>(Showable);
	UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (!CanvasSlot)
		return FVector2D::ZeroVector;
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetOffsets(FMargin(0.0f));
	CanvasSlot->SetZOrder(ZOrder);
	if (Placement == EKishiShowablePlacement::Fill)
	{
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		return FVector2D::ZeroVector;
	}
	const FVector2D OldPosition = CanvasSlot->GetAnchors().Minimum - FVector2D(0.5f, 1.0f);
	// Script positions are fractions of the canvas from its bottom center. Size the widget to its content and pin its bottom
	// center there, so it follows the canvas at any resolution.
	const FVector2D Anchor = FVector2D(0.5f, 1.0f) + Position;
	CanvasSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
	CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
	const FVector2D CanvasSize = Canvas ? Canvas->GetCachedGeometry().GetLocalSize() : FVector2D::ZeroVector;
	return (OldPosition - Position) * CanvasSize;
}

bool UKishiShowableLayer::IsWidgetShowableAt(UObject *Showable, FVector2D Position) const
{
	const UWidget *Widget = Cast<UWidget>(Showable);
	const UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	return !CanvasSlot || CanvasSlot->GetAnchors().Minimum.Equals(FVector2D(0.5f, 1.0f) + Position, KINDA_SMALL_NUMBER);
}

FVector2D UKishiShowableLayer::GetOffscreenOffset(UObject *Showable, ERPYTransitionDirection Direction) const
{
	const UWidget *Widget = Cast<UWidget>(Showable);
	const UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (!CanvasSlot || !Canvas || Direction == ERPYTransitionDirection::NONE)
		return FVector2D::ZeroVector;
	const FVector2D CanvasSize = Canvas->GetCachedGeometry().GetLocalSize();
	// The slot pins the widget's alignment point to its anchor; the render scale shrinks or grows it around that point.
	const FVector2D Pin = CanvasSlot->GetAnchors().Minimum * CanvasSize;
	const FVector2D Size = Widget->GetDesiredSize() * Widget->RenderTransform.Scale;
	const FVector2D TopLeft = Pin - CanvasSlot->GetAlignment() * Size;
	switch (Direction)
	{
	case ERPYTransitionDirection::LEFT:
		return FVector2D(-(TopLeft.X + Size.X), 0.0f);
	case ERPYTransitionDirection::RIGHT:
		return FVector2D(CanvasSize.X - TopLeft.X, 0.0f);
	case ERPYTransitionDirection::UP:
		return FVector2D(0.0f, -(TopLeft.Y + Size.Y));
	case ERPYTransitionDirection::DOWN:
		return FVector2D(0.0f, CanvasSize.Y - TopLeft.Y);
	default:
		return FVector2D::ZeroVector;
	}
}

void UKishiShowableLayer::FitWidgetShowable(UObject *Showable) const
{
	UWidget *Widget = Cast<UWidget>(Showable);
	const UCanvasPanelSlot *CanvasSlot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr;
	if (!IsValid(Widget) || !Canvas || !CanvasSlot)
		return;
	const FVector2D CanvasSize = Canvas->GetCachedGeometry().GetLocalSize();
	// The content was just set, so measure it now instead of waiting for the next layout pass.
	Widget->ForceLayoutPrepass();
	const FVector2D ContentSize = Widget->GetDesiredSize();
	FittedContentSizes.Add(Widget, ContentSize);
	float Scale = 1.0f;
	if (CanvasSize.X > 0.0f && CanvasSize.Y > 0.0f && ContentSize.X > 0.0f && ContentSize.Y > 0.0f)
	{
		if (Placement == EKishiShowablePlacement::Fill)
			Scale = FMath::Max(CanvasSize.X / ContentSize.X, CanvasSize.Y / ContentSize.Y);
		else if (bFitToCanvas)
			Scale = FMath::Min3(1.0f, CanvasSize.X / ContentSize.X, CanvasSize.Y / ContentSize.Y);
	}
	// Scale around the point the slot pins to the canvas.
	Widget->SetRenderTransformPivot(CanvasSlot->GetAlignment());
	Widget->SetRenderScale(FVector2D(Scale));
	VNSHOW_LOG("fit %s: content=%s canvas=%s scale=%.3f", *Widget->GetName(), *ContentSize.ToString(), *CanvasSize.ToString(), Scale);
}

void UKishiShowableLayer::PlayShowableTransition(UObject *Showable, const FRpySceneOptions &Options, bool bShow, FVector2D MoveFrom, bool bEnters)
{
	if (!IsValid(Showable) || !Showable->GetClass()->ImplementsInterface(UShowable::StaticClass()))
		return;
	FKishiShowableOptions ShowableOptions = URpyShowManagerLibrary::MakeShowableOptions(Options);
	const bool bTravels = ShowableOptions.Type == EKishiAnimationType::Move || ShowableOptions.Type == EKishiAnimationType::Ease;
	if (bShow && bTravels && bEnters && Options.direction != ERPYTransitionDirection::NONE)
	{
		// An entering image slides in from past the canvas edge named by the transition, such as easeright.
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
	VNSHOW_LOG("%s transition on %s: type=%s duration=%.2f animateOpacity=%d useFrom=%d from=%s", bShow ? TEXT("show") : TEXT("hide"), *Showable->GetName(),
			   *UEnum::GetValueAsString(ShowableOptions.Type), ShowableOptions.Duration, ShowableOptions.bAnimateOpacity ? 1 : 0, ShowableOptions.bUseFrom ? 1 : 0,
			   *ShowableOptions.From.ToString());
	if (bShow)
		IShowable::Execute_Show(Showable, ShowableOptions);
	else
		IShowable::Execute_Hide(Showable, ShowableOptions);
}

#undef VNSHOW_LOG
