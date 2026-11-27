#pragma once

#include "CoreMinimal.h"
#include "Script/RpyScriptTypes.h"

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
	/** Videos only. 0 plays the video once and holds its last frame; a positive value loops the last that many seconds; a negative value loops the whole video. Script: loop="0.2", loop="true". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float loop = 0.0f;
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
