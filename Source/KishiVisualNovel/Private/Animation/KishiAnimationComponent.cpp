#include "Animation/KishiAnimationComponent.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/Widget.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/Animatable.h"

UKishiAnimationComponent::UKishiAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKishiAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

FVector UKishiAnimationComponent::GetCurrentPosition(EKishiAnimationSpace Space) const
{
	if (Space == EKishiAnimationSpace::World)
	{
		return IsValid(WorldTarget) ? WorldTarget->GetComponentLocation() : FVector::ZeroVector;
	}
	if (Space == EKishiAnimationSpace::Viewport && !IsValid(WidgetTarget) && IsValid(WorldTarget))
	{
		APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		FVector2D ScreenPosition;
		if (PlayerController && PlayerController->ProjectWorldLocationToScreen(WorldTarget->GetComponentLocation(), ScreenPosition, true))
		{
			return FVector(ScreenPosition.X, ScreenPosition.Y, 0.0f);
		}
		return FVector::ZeroVector;
	}
	if (!IsValid(WidgetTarget))
	{
		return FVector::ZeroVector;
	}
	const FVector2D Translation = WidgetTarget->RenderTransform.Translation;
	if (Space == EKishiAnimationSpace::Viewport)
	{
		const float ViewportScale = FMath::Max(UWidgetLayoutLibrary::GetViewportScale(WidgetTarget), KINDA_SMALL_NUMBER);
		return FVector(Translation.X * ViewportScale, Translation.Y * ViewportScale, 0.0f);
	}
	return FVector(Translation.X, Translation.Y, 0.0f);
}

void UKishiAnimationComponent::Animate_Implementation(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration)
{
	if (IsAnimating_Implementation())
	{
		UE_LOG(LogTemp, Log, TEXT("KishiAnimation: skipping active animation before starting the next one."));
		IAnimatable::Execute_Skip(this);
	}

	const bool bNeedsSceneTarget = Space == EKishiAnimationSpace::World || (Space == EKishiAnimationSpace::Viewport && !IsValid(WidgetTarget));
	if ((bNeedsSceneTarget && !IsValid(WorldTarget)) || (!bNeedsSceneTarget && !IsValid(WidgetTarget)))
	{
		UE_LOG(LogTemp, Warning, TEXT("KishiAnimation: no valid target for the selected animation space."));
		return;
	}

	ActiveSpace = Space;
	ActiveType = Type;
	UE_LOG(LogTemp, Log, TEXT("KishiAnimation: starting %s animation in %s space for %.3f seconds after %.3f seconds delay."),
		*UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Space), FMath::Max(Duration, 0.0f), FMath::Max(Delay, 0.0f));
	StartPosition = bUseFrom ? From : GetCurrentPosition(Space);
	EndPosition = To;
	Elapsed = 0.0f;
	DelayRemaining = FMath::Max(Delay, 0.0f);
	DurationSeconds = FMath::Max(Duration, 0.0f);
	bWaitingForDelay = DelayRemaining > 0.0f;
	bIsAnimating = !bWaitingForDelay;

	if (Space == EKishiAnimationSpace::World || (Space == EKishiAnimationSpace::Viewport && !IsValid(WidgetTarget)))
	{
		OriginalScale = WorldTarget->GetComponentScale();
		OriginalWorldRotation = WorldTarget->GetComponentRotation();
	}
	else
	{
		const FWidgetTransform Transform = WidgetTarget->RenderTransform;
		OriginalWidgetScale = Transform.Scale;
		OriginalWidgetPivot = WidgetTarget->RenderTransformPivot;
		OriginalOpacity = WidgetTarget->GetRenderOpacity();
		if (ActiveType == EKishiAnimationType::Fade && OriginalOpacity <= 0.0f && !bFadeOut)
		{
			OriginalOpacity = 1.0f;
		}
	}

	SetComponentTickEnabled(true);
	if (!bWaitingForDelay && DurationSeconds <= 0.0f)
	{
		ApplyPosition(EndPosition, 1.0f);
		FinishAnimation();
	}
}

void UKishiAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bWaitingForDelay)
	{
		DelayRemaining -= DeltaTime;
		if (DelayRemaining > 0.0f)
		{
			return;
		}
		bWaitingForDelay = false;
		bIsAnimating = true;
		if (DurationSeconds <= 0.0f)
		{
			ApplyPosition(EndPosition, 1.0f);
			FinishAnimation();
			return;
		}
	}
	if (!bIsAnimating)
	{
		SetComponentTickEnabled(false);
		return;
	}

	Elapsed = FMath::Min(Elapsed + FMath::Max(DeltaTime, 0.0f), DurationSeconds);
	const float LinearProgress = DurationSeconds > 0.0f ? Elapsed / DurationSeconds : 1.0f;
	const float Progress = ActiveType == EKishiAnimationType::Ease || ActiveType == EKishiAnimationType::Move
		? GetEasedProgress(LinearProgress) : LinearProgress;
	ApplyPosition(FMath::Lerp(StartPosition, EndPosition, Progress), Progress);
	if (LinearProgress >= 1.0f)
	{
		FinishAnimation();
	}
}

float UKishiAnimationComponent::GetEasedProgress(float Progress) const
{
	return FMath::InterpEaseInOut(0.0f, 1.0f, FMath::Clamp(Progress, 0.0f, 1.0f), 2.0f);
}

void UKishiAnimationComponent::ApplyPosition(const FVector& Position, float Progress)
{
	const float Sine = FMath::Sin(Progress * PI * 8.0f) * (1.0f - Progress);
	if (ActiveSpace == EKishiAnimationSpace::World || (ActiveSpace == EKishiAnimationSpace::Viewport && !IsValid(WidgetTarget)))
	{
		if (!IsValid(WorldTarget)) return;
		FVector Applied = Position;
		if (ActiveSpace == EKishiAnimationSpace::Viewport)
		{
			APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
			FVector RayOrigin;
			FVector RayDirection;
			FVector ViewOrigin;
			FRotator ViewRotation;
			if (!PlayerController || !PlayerController->DeprojectScreenPositionToWorld(Position.X, Position.Y, RayOrigin, RayDirection)) return;
			PlayerController->GetPlayerViewPoint(ViewOrigin, ViewRotation);
			const FVector PlaneNormal = ViewRotation.Vector();
			const float PlaneDepth = Position.Z > 0.0f
				? Position.Z
				: FVector::DotProduct(WorldTarget->GetComponentLocation() - ViewOrigin, PlaneNormal);
			const float RayPlaneDot = FVector::DotProduct(RayDirection, PlaneNormal);
			if (FMath::Abs(RayPlaneDot) <= KINDA_SMALL_NUMBER) return;
			const FVector PlanePoint = ViewOrigin + PlaneNormal * PlaneDepth;
			const float RayDistance = FVector::DotProduct(PlanePoint - RayOrigin, PlaneNormal) / RayPlaneDot;
			Applied = RayOrigin + RayDirection * RayDistance;
		}
		if (ActiveType == EKishiAnimationType::Shake) Applied += FVector(0.0f, Sine * 12.0f, 0.0f);
		WorldTarget->SetWorldLocation(Applied);
		if (ActiveType == EKishiAnimationType::Spin)
		{
			WorldTarget->SetWorldRotation(OriginalWorldRotation + FRotator(0.0f, 360.0f * Progress, 0.0f));
		}
		float ScaleFactor = 1.0f;
		if (ActiveType == EKishiAnimationType::Zoom) ScaleFactor = FMath::Lerp(0.1f, 1.0f, Progress);
		else if (ActiveType == EKishiAnimationType::Bounce) ScaleFactor = 1.0f + FMath::Abs(Sine) * 0.35f;
		else if (ActiveType == EKishiAnimationType::Pulse) ScaleFactor = 1.0f + FMath::Sin(Progress * PI) * 0.2f;
		WorldTarget->SetWorldScale3D(OriginalScale * ScaleFactor);
		return;
	}
	if (!IsValid(WidgetTarget)) return;
	FVector2D Translation(Position.X, Position.Y);
	if (ActiveSpace == EKishiAnimationSpace::Viewport)
	{
		const float ViewportScale = FMath::Max(UWidgetLayoutLibrary::GetViewportScale(WidgetTarget), KINDA_SMALL_NUMBER);
		Translation /= ViewportScale;
	}
	if (ActiveType == EKishiAnimationType::Shake) Translation.X += Sine * 12.0f;
	WidgetTarget->SetRenderTranslation(Translation);

	float ScaleFactor = 1.0f;
	if (ActiveType == EKishiAnimationType::Zoom) ScaleFactor = FMath::Lerp(0.1f, 1.0f, Progress);
	else if (ActiveType == EKishiAnimationType::Bounce) ScaleFactor = 1.0f + FMath::Abs(Sine) * 0.35f;
	else if (ActiveType == EKishiAnimationType::Pulse) ScaleFactor = 1.0f + FMath::Sin(Progress * PI) * 0.2f;
	FVector2D AnimatedScale = OriginalWidgetScale * ScaleFactor;
	if (ActiveType == EKishiAnimationType::Wipe)
	{
		WidgetTarget->SetRenderTransformPivot(FVector2D(0.0f, 0.5f));
		AnimatedScale.X *= Progress;
	}
	else if (ActiveType == EKishiAnimationType::Blur)
	{
		AnimatedScale *= FMath::Lerp(1.15f, 1.0f, Progress);
	}
	WidgetTarget->SetRenderScale(AnimatedScale);

	if (ActiveType == EKishiAnimationType::Fade || ActiveType == EKishiAnimationType::Dissolve || ActiveType == EKishiAnimationType::Blur)
	{
		WidgetTarget->SetRenderOpacity(OriginalOpacity * (bFadeOut ? 1.0f - Progress : Progress));
	}
	if (ActiveType == EKishiAnimationType::Spin)
	{
		FWidgetTransform Transform = WidgetTarget->RenderTransform;
		Transform.Angle = 360.0f * Progress;
		WidgetTarget->SetRenderTransform(Transform);
	}
}

void UKishiAnimationComponent::Skip_Implementation()
{
	if (!IsAnimating_Implementation()) return;
	ApplyPosition(EndPosition, 1.0f);
	FinishAnimation();
}

void UKishiAnimationComponent::FinishAnimation()
{
	bIsAnimating = false;
	bWaitingForDelay = false;
	if (bHideAfterAnimation)
	{
		bHideAfterAnimation = false;
		if (IsValid(WorldTarget))
		{
			WorldTarget->SetVisibility(false, true);
			if (AActor* OwnerActor = WorldTarget->GetOwner()) OwnerActor->SetActorHiddenInGame(true);
		}
		if (IsValid(WidgetTarget)) WidgetTarget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ActiveSpace == EKishiAnimationSpace::World && IsValid(WorldTarget))
	{
		WorldTarget->SetWorldScale3D(OriginalScale);
		if (ActiveType == EKishiAnimationType::Spin)
		{
			WorldTarget->SetWorldRotation(OriginalWorldRotation);
		}
	}
	else if (IsValid(WidgetTarget))
	{
		WidgetTarget->SetRenderScale(OriginalWidgetScale);
		WidgetTarget->SetRenderTransformPivot(OriginalWidgetPivot);
		if (ActiveType == EKishiAnimationType::Spin)
		{
			FWidgetTransform Transform = WidgetTarget->RenderTransform;
			Transform.Angle = 0.0f;
			WidgetTarget->SetRenderTransform(Transform);
		}
	}
	SetComponentTickEnabled(false);
	IAnimatable::Execute_HandleAnimationEnded(this);
}

void UKishiAnimationComponent::Show_Implementation(const FKishiShowableOptions& Options)
{
	if (IsAnimating_Implementation()) IAnimatable::Execute_Skip(this);
	bHideAfterAnimation = false;
	bFadeOut = false;
	if (IsValid(WorldTarget))
	{
		WorldTarget->SetVisibility(true, true);
		if (AActor* OwnerActor = WorldTarget->GetOwner()) OwnerActor->SetActorHiddenInGame(false);
	}
	if (IsValid(WidgetTarget))
	{
		WidgetTarget->SetVisibility(ESlateVisibility::Visible);
		if (Options.bAnimateOpacity) WidgetTarget->SetRenderOpacity(0.0f);
	}
	IAnimatable::Execute_Animate(this, Options.From, Options.To, Options.bUseFrom, Options.bAnimateOpacity ? EKishiAnimationType::Fade : Options.Type, Options.Delay, Options.Space, Options.Duration);
}

void UKishiAnimationComponent::Hide_Implementation(const FKishiShowableOptions& Options)
{
	if (IsAnimating_Implementation()) IAnimatable::Execute_Skip(this);
	bHideAfterAnimation = true;
	bFadeOut = IsValid(WidgetTarget) && (Options.bAnimateOpacity || Options.Type == EKishiAnimationType::Fade || Options.Type == EKishiAnimationType::Dissolve || Options.Type == EKishiAnimationType::Blur);
	IAnimatable::Execute_Animate(this, Options.From, Options.To, Options.bUseFrom, bFadeOut ? EKishiAnimationType::Fade : Options.Type, Options.Delay, Options.Space, Options.Duration);
}

bool UKishiAnimationComponent::IsShown_Implementation() const
{
	if (IsValid(WidgetTarget))
	{
		return WidgetTarget->GetVisibility() != ESlateVisibility::Collapsed && WidgetTarget->GetVisibility() != ESlateVisibility::Hidden;
	}
	if (!IsValid(WorldTarget)) return false;
	const AActor* OwnerActor = WorldTarget->GetOwner();
	return WorldTarget->IsVisible() && (!OwnerActor || !OwnerActor->IsHidden());
}

void UKishiAnimationComponent::HandleAnimationEnded_Implementation()
{
	OnAnimationEnded.Broadcast();
}
