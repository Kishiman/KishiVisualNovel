#include "Animation/Transitions/RpyZoomTransition.h"

#include "Components/SceneComponent.h"
#include "Components/Widget.h"

void URpyZoomTransition::ApplyValue(FVector2D Value)
{
	if (UWidget *TargetWidget = WidgetTarget.Get())
	{
		TargetWidget->SetRenderScale(Value);
	}
	else if (USceneComponent *TargetSceneComponent = SceneTarget.Get())
	{
		TargetSceneComponent->SetWorldScale3D(FVector(Value.X, Value.Y, InitialWorldScale.Z));
	}
}

float URpyZoomEaseTransition::MapProgress(float LinearProgress) const
{
	return FMath::InterpEaseInOut(0.0f, 1.0f, LinearProgress, 2.0f);
}
