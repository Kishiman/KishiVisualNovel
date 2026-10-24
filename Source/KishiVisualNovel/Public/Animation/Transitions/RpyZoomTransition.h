#pragma once

#include "Animation/Transitions/RpyTransitionBase.h"
#include "RpyZoomTransition.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyZoomTransition : public URpyTransitionBase
{
	GENERATED_BODY()

protected:
	virtual void ApplyValue(FVector2D Value) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyZoomEaseTransition : public URpyZoomTransition
{
	GENERATED_BODY()

protected:
	virtual float MapProgress(float LinearProgress) const override;
};
