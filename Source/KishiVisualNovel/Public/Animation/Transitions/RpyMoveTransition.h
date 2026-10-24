#pragma once

#include "Animation/Transitions/RpyTransitionBase.h"
#include "RpyMoveTransition.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyMoveTransition : public URpyTransitionBase
{
	GENERATED_BODY()

protected:
	virtual void ApplyValue(FVector2D Value) override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyMoveEaseTransition : public URpyMoveTransition
{
	GENERATED_BODY()

protected:
	virtual float MapProgress(float LinearProgress) const override;
};
