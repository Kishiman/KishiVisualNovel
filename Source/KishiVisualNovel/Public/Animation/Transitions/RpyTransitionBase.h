#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RpyTransitionBase.generated.h"

class UCameraComponent;
class USceneComponent;
class UWidget;

/** Base for manually-ticked RPY transitions. Keep the legacy vector solver for existing widgets. */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class KISHIVISUALNOVEL_API URpyTransitionBase : public UObject
{
	GENERATED_BODY()

public:
	/** Start in UMG space. The meaning of Start and End depends on the derived effect. */
	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation|Transition")
	void Start2D(UWidget *Target, FVector2D Start, FVector2D End, float Duration);

	/** Start a scene-component transition with a reference camera. Move positions use normalized [-1, 1] coordinates. */
	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation|Transition")
	void Start3D(USceneComponent *Target, UCameraComponent *ReferenceCamera, FVector2D Start, FVector2D End, float PlaneDepth, float Duration);

	/** Advance the transition. Returns true once the transition has completed. */
	UFUNCTION(BlueprintCallable, Category = "Visual Novel|Animation|Transition")
	bool TickTransition(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Animation|Transition")
	bool IsTransitionActive() const { return bIsActive; }

	UFUNCTION(BlueprintPure, Category = "Visual Novel|Animation|Transition")
	float GetTransitionProgress() const;

protected:
	virtual void ApplyValue(FVector2D Value);
	virtual float MapProgress(float LinearProgress) const;
	FVector GetCameraPlaneLocation(FVector2D NormalizedPosition) const;

	TWeakObjectPtr<UWidget> WidgetTarget;
	TWeakObjectPtr<USceneComponent> SceneTarget;
	TWeakObjectPtr<UCameraComponent> ReferenceCamera;
	FVector2D StartValue = FVector2D::ZeroVector;
	FVector2D EndValue = FVector2D::ZeroVector;
	FVector InitialWorldScale = FVector::OneVector;
	float PlaneDepth = 1000.0f;
	float DurationSeconds = 0.0f;
	float ElapsedSeconds = 0.0f;
	bool bIsActive = false;
};
