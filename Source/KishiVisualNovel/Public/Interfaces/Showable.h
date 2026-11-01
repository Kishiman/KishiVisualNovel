#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Animatable.h"
#include "Showable.generated.h"

/** Common presentation settings accepted by showable objects. */
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
	EKishiAnimationType Type = EKishiAnimationType::Fade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable", meta=(ClampMin="0.0"))
	float Delay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	EKishiAnimationSpace Space = EKishiAnimationSpace::ParentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable", meta=(ClampMin="0.0"))
	float Duration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Showable")
	bool bAnimateOpacity = true;

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
};
