#include "Animation/Transitions/RpyTransitionBase.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/Widget.h"

void URpyTransitionBase::Start2D(UWidget *Target, FVector2D Start, FVector2D End, float Duration)
{
	WidgetTarget = Target;
	SceneTarget.Reset();
	ReferenceCamera.Reset();
	StartValue = Start;
	EndValue = End;
	DurationSeconds = FMath::Max(Duration, 0.0f);
	ElapsedSeconds = 0.0f;
	bIsActive = IsValid(Target);
	if (bIsActive && DurationSeconds == 0.0f)
	{
		ApplyValue(EndValue);
		bIsActive = false;
	}
}

void URpyTransitionBase::Start3D(USceneComponent *Target, UCameraComponent *Camera, FVector2D Start, FVector2D End, float Depth, float Duration)
{
	WidgetTarget.Reset();
	SceneTarget = Target;
	ReferenceCamera = Camera;
	StartValue = Start;
	EndValue = End;
	PlaneDepth = FMath::Max(Depth, 1.0f);
	DurationSeconds = FMath::Max(Duration, 0.0f);
	ElapsedSeconds = 0.0f;
	bIsActive = IsValid(Target) && IsValid(Camera);
	if (bIsActive)
	{
		InitialWorldScale = Target->GetComponentScale();
	}
	if (bIsActive && DurationSeconds == 0.0f)
	{
		ApplyValue(EndValue);
		bIsActive = false;
	}
}

bool URpyTransitionBase::TickTransition(float DeltaSeconds)
{
	if (!bIsActive)
	{
		return true;
	}
	if (!WidgetTarget.IsValid() && (!SceneTarget.IsValid() || !ReferenceCamera.IsValid()))
	{
		bIsActive = false;
		return true;
	}

	ElapsedSeconds = FMath::Min(ElapsedSeconds + FMath::Max(DeltaSeconds, 0.0f), DurationSeconds);
	const float LinearProgress = DurationSeconds > 0.0f ? ElapsedSeconds / DurationSeconds : 1.0f;
	const float Progress = FMath::Clamp(MapProgress(LinearProgress), 0.0f, 1.0f);
	ApplyValue(FMath::Lerp(StartValue, EndValue, Progress));
	if (LinearProgress >= 1.0f)
	{
		bIsActive = false;
		return true;
	}
	return false;
}

float URpyTransitionBase::GetTransitionProgress() const
{
	return DurationSeconds > 0.0f ? FMath::Clamp(ElapsedSeconds / DurationSeconds, 0.0f, 1.0f) : (bIsActive ? 0.0f : 1.0f);
}

void URpyTransitionBase::ApplyValue(FVector2D Value)
{
	// Effect-specific subclasses apply the value to their target.
	static_cast<void>(Value);
}

float URpyTransitionBase::MapProgress(float LinearProgress) const
{
	return LinearProgress;
}

FVector URpyTransitionBase::GetCameraPlaneLocation(FVector2D NormalizedPosition) const
{
	const UCameraComponent *Camera = ReferenceCamera.Get();
	if (!Camera)
	{
		return FVector::ZeroVector;
	}

	const float AspectRatio = FMath::Max(Camera->AspectRatio, KINDA_SMALL_NUMBER);
	const float HalfWidth = Camera->ProjectionMode == ECameraProjectionMode::Orthographic
		? Camera->OrthoWidth * 0.5f
		: PlaneDepth * FMath::Tan(FMath::DegreesToRadians(Camera->FieldOfView * 0.5f));
	const float HalfHeight = HalfWidth / AspectRatio;
	const FTransform CameraTransform = Camera->GetComponentTransform();
	return CameraTransform.GetLocation()
		+ CameraTransform.GetUnitAxis(EAxis::X) * PlaneDepth
		+ CameraTransform.GetUnitAxis(EAxis::Y) * (NormalizedPosition.X * HalfWidth)
		+ CameraTransform.GetUnitAxis(EAxis::Z) * (NormalizedPosition.Y * HalfHeight);
}
