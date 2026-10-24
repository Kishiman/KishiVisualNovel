#pragma once

#include "Animation/Transitions/RpyTransitionBase.h"
#include "RpyFadeTransition.generated.h"

/** UMG opacity transition. Pass opacity values in the X component of Start and End. */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyFadeTransition : public URpyTransitionBase
{
	GENERATED_BODY()

protected:
	virtual void ApplyValue(FVector2D Value) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyFadeEaseTransition : public URpyFadeTransition
{
	GENERATED_BODY()

protected:
	virtual float MapProgress(float LinearProgress) const override;
};
