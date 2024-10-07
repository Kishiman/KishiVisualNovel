// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KishiDataAsset.h"
#include "Layerable.h"
#include "PaperSprite.h"
#include "LayeredSprite.generated.h"

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
	FName name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool displayed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite *sprite;
};
UCLASS(BlueprintType)
class KISHIVISUALNOVEL_API ULayeredSprite : public UKishiDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSpriteLayer> layers;

	UFUNCTION(BlueprintCallable, Category = "LayeredSprite")
	bool FindOnlyLayerByName(FName LayerName) const;

	UFUNCTION(BlueprintCallable, Category = "LayeredSprite")
	bool FindLayerByName(FName LayerName, FSpriteLayer &layer, TArray<FSpriteLayer> &groupLayers) const;

	UFUNCTION(BlueprintPure, Category = "LayeredSprite")
	void FindLayersByAttribute(FString Attribute, TArray<FSpriteLayer> &FoundLayers, TArray<FSpriteLayer> &groupLayers) const;

	UFUNCTION(BlueprintCallable, Category = "LayeredSprite")
	void FindLayersByGroup(FName GroupName, TArray<FSpriteLayer> &groupLayers) const;

	UFUNCTION(BlueprintCallable, Category = "LayeredSprite")
	bool DisplayLayer(FName LayerName);

	UFUNCTION(BlueprintCallable, Category = "LayeredSprite")
	static FName GetNameFromAssetName(FString AssetName);
};
