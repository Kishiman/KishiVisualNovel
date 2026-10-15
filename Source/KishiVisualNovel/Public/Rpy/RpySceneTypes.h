#pragma once

#include "CoreMinimal.h"
#include "Rpy/RpyScriptTypes.h"

#include "RpySceneTypes.generated.h"

USTRUCT(BlueprintType)
struct FRpySceneOptions
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName layer = FName("master");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERPYTransitionType with = ERPYTransitionType::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyTransition transition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERPYTransitionDirection direction = ERPYTransitionDirection::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPositionType positionType = EPositionType::NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName positionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector position = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float transitionTime = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int zorder = 0;
};
USTRUCT(BlueprintType)
struct FRpySceneManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyImage rpyImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneOptions options;
};

USTRUCT(BlueprintType)
struct FRpyShowImageState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpyImage rpyImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRpySceneOptions options;
};

USTRUCT(BlueprintType)
struct FRpyShowManagerState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRpyShowImageState> imageStates;
};
