#include "Widgets/KishiShowableWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Interfaces/Animatable.h"

// Temporary transition and visibility tracing. Filter the Output Log on "[VNShow]".
#define VNSHOW_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("[VNShow] %s: " Format), *GetName(), ##__VA_ARGS__)

UKishiShowableWidget::UKishiShowableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKishiShowableWidget::Animate_Implementation(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration)
{
	if (IsAnimating_Implementation())
	{
		UE_LOG(LogTemp, Log, TEXT("KishiShowableWidget: skipping active animation before starting the next one."));
		Skip_Implementation();
	}
	if (Space == EKishiAnimationSpace::World)
	{
		UE_LOG(LogTemp, Warning, TEXT("KishiShowableWidget: world-space animation requires UKishiAnimationComponent."));
		return;
	}

	const float ViewportScale = Space == EKishiAnimationSpace::Viewport
		? FMath::Max(UWidgetLayoutLibrary::GetViewportScale(this), KINDA_SMALL_NUMBER) : 1.0f;
	const FVector2D Current = RenderTransform.Translation * ViewportScale;
	StartPosition = bUseFrom ? FVector2D(From.X, From.Y) : Current;
	EndPosition = FVector2D(To.X, To.Y);
	if (Space == EKishiAnimationSpace::Viewport)
	{
		StartPosition /= ViewportScale;
		EndPosition /= ViewportScale;
	}
	ActiveType = Type;
	DelayRemaining = FMath::Max(Delay, 0.0f);
	Elapsed = 0.0f;
	DurationSeconds = FMath::Max(Duration, 0.0f);
	bWaitingForDelay = DelayRemaining > 0.0f;
	bIsAnimating = !bWaitingForDelay;
	OriginalScale = RenderTransform.Scale;
	OriginalPivot = RenderTransformPivot;
	OriginalOpacity = GetRenderOpacity();
	if (ActiveType == EKishiAnimationType::Fade && OriginalOpacity <= 0.0f && !bFadeOut)
	{
		OriginalOpacity = 1.0f;
	}

	VNSHOW_LOG("animate type=%s from=%s to=%s delay=%.2f duration=%.2f space=%s fadeOut=%d hideAfter=%d scale=%s pivot=%s opacity=%.2f",
						 *UEnum::GetValueAsString(ActiveType), *StartPosition.ToString(), *EndPosition.ToString(), DelayRemaining, DurationSeconds,
						 *UEnum::GetValueAsString(Space), bFadeOut ? 1 : 0, bHideAfterAnimation ? 1 : 0, *OriginalScale.ToString(), *OriginalPivot.ToString(), OriginalOpacity);
	if (!bWaitingForDelay && DurationSeconds <= 0.0f)
	{
		ApplyAnimationValue(1.0f);
		FinishAnimation();
		return;
	}
	// Slate arranges this frame before the next tick, so apply the start state now or the end state flashes for a frame.
	ApplyAnimationValue(0.0f);
}

void UKishiShowableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bWaitingForDelay)
	{
		DelayRemaining -= InDeltaTime;
		if (DelayRemaining > 0.0f) return;
		bWaitingForDelay = false;
		bIsAnimating = true;
		if (DurationSeconds <= 0.0f)
		{
			ApplyAnimationValue(1.0f);
			FinishAnimation();
			return;
		}
	}
	if (!bIsAnimating) return;
	Elapsed = FMath::Min(Elapsed + FMath::Max(InDeltaTime, 0.0f), DurationSeconds);
	const float LinearProgress = DurationSeconds > 0.0f ? Elapsed / DurationSeconds : 1.0f;
	float Progress = LinearProgress;
	if (ActiveType == EKishiAnimationType::Ease || ActiveType == EKishiAnimationType::Move)
	{
		Progress = FMath::InterpEaseInOut(0.0f, 1.0f, LinearProgress, 2.0f);
	}
	ApplyAnimationValue(Progress);
	if (LinearProgress >= 1.0f) FinishAnimation();
}

void UKishiShowableWidget::ApplyAnimationValue(float Progress)
{
	const float Sine = FMath::Sin(Progress * PI * 8.0f) * (1.0f - Progress);
	FVector2D Position = FMath::Lerp(StartPosition, EndPosition, Progress);
	if (ActiveType == EKishiAnimationType::Shake) Position.X += Sine * 12.0f;
	SetRenderTranslation(Position);

	// Zoom, wipe and blur play in reverse when hiding, so the widget shrinks or closes before it collapses.
	const float Revealed = bHideAfterAnimation ? 1.0f - Progress : Progress;
	float ScaleFactor = 1.0f;
	if (ActiveType == EKishiAnimationType::Zoom) ScaleFactor = FMath::Lerp(0.1f, 1.0f, Revealed);
	else if (ActiveType == EKishiAnimationType::Bounce) ScaleFactor = 1.0f + FMath::Abs(Sine) * 0.35f;
	else if (ActiveType == EKishiAnimationType::Pulse) ScaleFactor = 1.0f + FMath::Sin(Progress * PI) * 0.2f;
	FVector2D Scale = OriginalScale * ScaleFactor;
	if (ActiveType == EKishiAnimationType::Wipe)
	{
		SetRenderTransformPivot(FVector2D(0.0f, 0.5f));
		Scale.X *= Revealed;
	}
	else if (ActiveType == EKishiAnimationType::Blur)
	{
		Scale *= FMath::Lerp(1.15f, 1.0f, Revealed);
	}
	SetRenderScale(Scale);

	if (ActiveType == EKishiAnimationType::Fade || ActiveType == EKishiAnimationType::Dissolve || ActiveType == EKishiAnimationType::Blur)
	{
		SetRenderOpacity(OriginalOpacity * (bFadeOut ? 1.0f - Progress : Progress));
	}
	if (ActiveType == EKishiAnimationType::Spin)
	{
		FWidgetTransform Transform = RenderTransform;
		Transform.Angle = 360.0f * Progress;
		SetRenderTransform(Transform);
	}
}

void UKishiShowableWidget::Skip_Implementation()
{
	if (!IsAnimating_Implementation()) return;
	ApplyAnimationValue(1.0f);
	FinishAnimation();
}

void UKishiShowableWidget::FinishAnimation()
{
	bIsAnimating = false;
	bWaitingForDelay = false;
	if (bHideAfterAnimation)
	{
		bHideAfterAnimation = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
	SetRenderScale(OriginalScale);
	SetRenderTransformPivot(OriginalPivot);
	if (ActiveType == EKishiAnimationType::Spin)
	{
		FWidgetTransform Transform = RenderTransform;
		Transform.Angle = 0.0f;
		SetRenderTransform(Transform);
	}
	VNSHOW_LOG("animation finished type=%s visibility=%s opacity=%.2f translation=%s scale=%s", *UEnum::GetValueAsString(ActiveType),
						 *UEnum::GetValueAsString(GetVisibility()), GetRenderOpacity(), *RenderTransform.Translation.ToString(), *RenderTransform.Scale.ToString());
	IAnimatable::Execute_HandleAnimationEnded(this);
}

void UKishiShowableWidget::HandleAnimationEnded_Implementation()
{
	OnAnimationEnded.Broadcast();
}

void UKishiShowableWidget::Show_Implementation(const FKishiShowableOptions& Options)
{
	if (IsAnimating_Implementation()) IAnimatable::Execute_Skip(this);
	bHideAfterAnimation = false;
	bFadeOut = false;
	// Not hit-testable itself, so a shown image never blocks clicks meant for the dialogue underneath.
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	// Restore full opacity unless fading in, since an instant hide leaves the widget at zero opacity.
	SetRenderOpacity(Options.bAnimateOpacity ? 0.0f : 1.0f);
	VNSHOW_LOG("show attributes='%s' type=%s animateOpacity=%d useFrom=%d -> visibility=%s opacity=%.2f", *Options.Attributes,
						 *UEnum::GetValueAsString(Options.Type), Options.bAnimateOpacity ? 1 : 0, Options.bUseFrom ? 1 : 0,
						 *UEnum::GetValueAsString(GetVisibility()), GetRenderOpacity());
	IAnimatable::Execute_Animate(this, Options.From, Options.To, Options.bUseFrom, Options.bAnimateOpacity ? EKishiAnimationType::Fade : Options.Type, Options.Delay, Options.Space, Options.Duration);
}

void UKishiShowableWidget::Hide_Implementation(const FKishiShowableOptions& Options)
{
	if (IsAnimating_Implementation()) IAnimatable::Execute_Skip(this);
	bHideAfterAnimation = true;
	bFadeOut = Options.bAnimateOpacity || Options.Type == EKishiAnimationType::Fade || Options.Type == EKishiAnimationType::Dissolve || Options.Type == EKishiAnimationType::Blur;
	VNSHOW_LOG("hide attributes='%s' type=%s fadeOut=%d, collapses when the animation ends", *Options.Attributes, *UEnum::GetValueAsString(Options.Type), bFadeOut ? 1 : 0);
	IAnimatable::Execute_Animate(this, Options.From, Options.To, Options.bUseFrom, Options.bAnimateOpacity ? EKishiAnimationType::Fade : Options.Type, Options.Delay, Options.Space, Options.Duration);
}
