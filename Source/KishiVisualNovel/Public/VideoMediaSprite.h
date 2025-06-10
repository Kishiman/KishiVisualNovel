// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "Utils/StringUtils.h"
#include "FileMediaSource.h"
#include "VideoMediaSprite.generated.h"


/**
 */

USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FMediaSection
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float startTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float endTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float loopStartTime;
};
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UVideoMediaSprite : public UKishiDataAsset
{
	GENERATED_BODY()
public:
//file media source
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFileMediaSource *fileMediaSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMediaSection> sections;


	UFUNCTION(BlueprintCallable, Category = "VideoMediaSprite")
	bool FindSectionByTime(float Time, FMediaSection &FoundSection) const;
};
