// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "Interfaces/Layerable.h"
#include "PaperSprite.h"
#include "PSDDataAsset.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpriteLayer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,category="Data")
	FString Path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,category="Data")
	UPaperSprite* Sprite;
};
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API UPSDDataAsset : public UKishiDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite,category="Data")
	TArray<FSpriteLayer>	SpriteLayers;
};
