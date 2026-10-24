#include "Animation/Transitions/RpyMoveTransition.h"

#include "Components/SceneComponent.h"
#include "Components/Widget.h"

void URpyMoveTransition::ApplyValue(FVector2D Value)
{
	if (UWidget *TargetWidget = WidgetTarget.Get())
	{
		TargetWidget->SetRenderTranslation(Value);
	}
	else if (USceneComponent *TargetSceneComponent = SceneTarget.Get())
	{
		TargetSceneComponent->SetWorldLocation(GetCameraPlaneLocation(Value));
	}
}

float URpyMoveEaseTransition::MapProgress(float LinearProgress) const
{
	return FMath::InterpEaseInOut(0.0f, 1.0f, LinearProgress, 2.0f);
}
