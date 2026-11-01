#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/Animatable.h"
#include "Interfaces/Showable.h"
#include "KishiShowableWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKishiWidgetAnimationEnded);

/** UMG implementation of IAnimatable for widget-local and viewport animations. */
UCLASS(BlueprintType, Blueprintable)
class KISHIVISUALNOVEL_API UKishiShowableWidget : public UUserWidget, public IShowable
{
	GENERATED_BODY()

public:
	UKishiShowableWidget(const FObjectInitializer& ObjectInitializer);

	virtual void Animate_Implementation(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration) override;
	virtual void Skip_Implementation() override;
	virtual bool IsAnimating_Implementation() const override { return bIsAnimating || bWaitingForDelay; }
	virtual void HandleAnimationEnded_Implementation() override;
	virtual void Show_Implementation(const FKishiShowableOptions& Options) override;
	virtual void Hide_Implementation(const FKishiShowableOptions& Options) override;
	virtual bool IsShown_Implementation() const override { return GetVisibility() != ESlateVisibility::Collapsed && GetVisibility() != ESlateVisibility::Hidden; }

	UPROPERTY(BlueprintAssignable, Category="Animation|Events")
	FKishiWidgetAnimationEnded OnAnimationEnded;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void ApplyAnimationValue(float Progress);
	void FinishAnimation();
	FVector2D StartPosition = FVector2D::ZeroVector;
	FVector2D EndPosition = FVector2D::ZeroVector;
	FVector2D OriginalScale = FVector2D::UnitVector;
	FVector2D OriginalPivot = FVector2D(0.5f, 0.5f);
	float OriginalOpacity = 1.0f;
	float DelayRemaining = 0.0f;
	float Elapsed = 0.0f;
	float DurationSeconds = 0.5f;
	EKishiAnimationType ActiveType = EKishiAnimationType::Linear;
	bool bIsAnimating = false;
	bool bWaitingForDelay = false;
	bool bHideAfterAnimation = false;
	bool bFadeOut = false;
};
