#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Animatable.h"
#include "Interfaces/Showable.h"
#include "KishiAnimationComponent.generated.h"

class USceneComponent;
class UWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKishiAnimationEnded);

/** Independent, tick-driven animation runner for a scene component or a UMG widget. */
UCLASS(ClassGroup=(Kishi), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class KISHIVISUALNOVEL_API UKishiAnimationComponent : public UActorComponent, public IShowable
{
	GENERATED_BODY()

public:
	UKishiAnimationComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UWidget* WidgetTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	USceneComponent* WorldTarget = nullptr;

	UPROPERTY(BlueprintAssignable, Category="Animation|Events")
	FKishiAnimationEnded OnAnimationEnded;

	/** Animate between positions in the selected space. Set bUseFrom=false to start at the target's current position. */
	virtual void Animate_Implementation(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration) override;

	/** Finish the current animation immediately and broadcast its completion event. */
	virtual void Skip_Implementation() override;

	virtual bool IsAnimating_Implementation() const override { return bIsAnimating || bWaitingForDelay; }
	virtual void HandleAnimationEnded_Implementation() override;
	virtual void Show_Implementation(const FKishiShowableOptions& Options) override;
	virtual void Hide_Implementation(const FKishiShowableOptions& Options) override;
	virtual bool IsShown_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FVector GetCurrentPosition(EKishiAnimationSpace Space) const;
	void ApplyPosition(const FVector& Position, float Progress);
	void FinishAnimation();
	float GetEasedProgress(float Progress) const;

	FVector StartPosition = FVector::ZeroVector;
	FVector EndPosition = FVector::ZeroVector;
	FVector OriginalScale = FVector::OneVector;
	FRotator OriginalWorldRotation = FRotator::ZeroRotator;
	FVector2D OriginalWidgetScale = FVector2D::UnitVector;
	FVector2D OriginalWidgetPivot = FVector2D(0.5f, 0.5f);
	float Elapsed = 0.0f;
	float DelayRemaining = 0.0f;
	float DurationSeconds = 0.5f;
	float OriginalOpacity = 1.0f;
	EKishiAnimationType ActiveType = EKishiAnimationType::Linear;
	EKishiAnimationSpace ActiveSpace = EKishiAnimationSpace::ParentWidget;
	bool bIsAnimating = false;
	bool bWaitingForDelay = false;
	bool bHideAfterAnimation = false;
	bool bFadeOut = false;
	bool bViewportTargetValid = true;
};
