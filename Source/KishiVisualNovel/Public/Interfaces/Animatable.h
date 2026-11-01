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
	Dissolve,
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Animation")
	void Animate(FVector From, FVector To, bool bUseFrom, EKishiAnimationType Type, float Delay, EKishiAnimationSpace Space, float Duration);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Animation")
	void Skip();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Animation")
	bool IsAnimating() const;

	/** Completion hook shared by native and Blueprint listeners. */
	UFUNCTION(BlueprintNativeEvent, Category="Animation|Events")
	void HandleAnimationEnded();
};
