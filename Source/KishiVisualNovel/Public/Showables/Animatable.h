#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Animatable.generated.h"

UENUM(BlueprintType)
enum class EKishiAnimationType : uint8
{
	Linear,
	Ease,
	Fade,
	Move,
	Zoom,
	Blur,
	Shake,
	Wipe,
	Bounce,
	Spin,
	Pulse
};

UENUM(BlueprintType)
enum class EKishiAnimationSpace : uint8
{
	World,
	Viewport,
	ParentWidget
};

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UAnimatable : public UInterface
{
	GENERATED_BODY()
};

/** Common animation contract for actor components and widgets. */
class KISHIVISUALNOVEL_API IAnimatable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IAnimatable|Required", meta = (ToolTip = "Required: animate to To (from From when bUseFrom) in Space over Duration, after Delay. Call HandleAnimationEnded when it finishes."))
	void Animate(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration);
	virtual void Animate_Implementation(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration) { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IAnimatable::Animate")); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IAnimatable|Required", meta = (ToolTip = "Required: finish the running animation immediately and call HandleAnimationEnded."))
	void Skip();
	virtual void Skip_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IAnimatable::Skip")); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IAnimatable|Required", meta = (ToolTip = "Required: return whether an animation is running."))
	bool IsAnimating() const;
	virtual bool IsAnimating_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IAnimatable::IsAnimating")); return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IAnimatable|Required", meta = (ToolTip = "Required: handler the implementer calls on itself when an animation finishes or is skipped; notify listeners. The Bases broadcast their OnAnimationEnded delegate."))
	void HandleAnimationEnded();
	virtual void HandleAnimationEnded_Implementation() { UE_LOG(LogTemp, Error, TEXT("Required Blueprint implementation missing: IAnimatable::HandleAnimationEnded")); }
};
