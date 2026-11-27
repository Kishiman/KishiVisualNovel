#pragma once

#include "CoreMinimal.h"
#include "Showables/Animatable.h"
#include "Showable.generated.h"

/** Common presentation settings accepted by showable objects. The defaults are a cut: no animation and no duration. */
USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FKishiShowableOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	FVector From = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	FVector To = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	bool bUseFrom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	EKishiAnimationType Type = EKishiAnimationType::Linear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable", meta=(ClampMin="0.0"))
	float Delay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	EKishiAnimationSpace Space = EKishiAnimationSpace::ParentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable", meta=(ClampMin="0.0"))
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	bool bAnimateOpacity = false;

	/** Video showables: 0 plays the video once and holds its last frame, a positive value loops its last that many seconds, a negative value loops all of it. Ignored by other showables. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	float MediaLoop = 0.0f;

	/** Layered-sprite attribute expression; ignored by other showables. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	FString Attributes;
};

UINTERFACE(BlueprintType)
class KISHIVISUALNOVEL_API UShowable : public UAnimatable
{
	GENERATED_BODY()
};

/** A showable animatable object that can be shown and hidden by a manager. */
class KISHIVISUALNOVEL_API IShowable : public IAnimatable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IShowable|Required", meta=(ToolTip="Required: show this object using the supplied presentation and attribute options."))
	void Show(const FKishiShowableOptions& Options);
	virtual void Show_Implementation(const FKishiShowableOptions& Options) { UE_LOG(LogTemp, Error, TEXT("Required IShowable::Show implementation missing on %s"), *GetNameSafe(_getUObject())); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IShowable|Required", meta=(ToolTip="Required: hide this object using the supplied presentation options."))
	void Hide(const FKishiShowableOptions& Options);
	virtual void Hide_Implementation(const FKishiShowableOptions& Options) { UE_LOG(LogTemp, Error, TEXT("Required IShowable::Hide implementation missing on %s"), *GetNameSafe(_getUObject())); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IShowable|Required", meta=(ToolTip="Required: report whether this object is currently visible."))
	bool IsShown() const;
	virtual bool IsShown_Implementation() const { UE_LOG(LogTemp, Error, TEXT("Required IShowable::IsShown implementation missing on %s"), *GetNameSafe(_getUObject())); return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IShowable|Required", meta=(ToolTip="Required: display Asset, an asset the showable registry in Project Settings maps to this class, fitted to TargetSize if this showable scales its content. Return false, after logging why, if this showable cannot display it."))
	bool SetShowableAsset(UObject* Asset, FVector2D TargetSize);
	virtual bool SetShowableAsset_Implementation(UObject* Asset, FVector2D TargetSize) { UE_LOG(LogTemp, Error, TEXT("Required IShowable::SetShowableAsset implementation missing on %s"), *GetNameSafe(_getUObject())); return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IShowable|Hooks", meta=(ToolTip="Optional: return true while the asset given to SetShowableAsset cannot be drawn yet, such as a video before its first frame. A showable layer then keeps this showable hidden and what is under it on screen, and starts its show transition once this returns false (or after 2 seconds). Returns false when not implemented."))
	bool IsLoading() const;
	virtual bool IsLoading_Implementation() const { return false; }
};
