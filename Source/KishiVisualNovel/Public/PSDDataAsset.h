// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "Layerable.h"
#include "PaperSprite.h"
#include "PSDDataAsset.generated.h"

/**
 */

USTRUCT(BlueprintType)
struct KISHIVISUALNOVEL_API FSpriteLayer
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName group;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool displayed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite *sprite;
};
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UPSDDataAsset : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSpriteLayer> layers;
};
