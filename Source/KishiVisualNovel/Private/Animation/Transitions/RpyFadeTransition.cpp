#include "Animation/Transitions/RpyFadeTransition.h"

#include "Components/Widget.h"

void URpyFadeTransition::ApplyValue(FVector2D Value)
{
	if (UWidget *TargetWidget = WidgetTarget.Get())
	{
		TargetWidget->SetRenderOpacity(Value.X);
	}
}

float URpyFadeEaseTransition::MapProgress(float LinearProgress) const
{
	return FMath::InterpEaseInOut(0.0f, 1.0f, LinearProgress, 2.0f);
}
